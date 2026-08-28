#!/usr/bin/env node
/**
 * WARLOCK LICENSE DESK
 * Warlock Audio / Warlock Plugins — factory tool only.
 *
 * This program ISSUES and VERIFIES signed license codes for
 * WARLOCK MONOLITH Model 1666 (and other Warlock products).
 *
 * The private key must NEVER ship inside the customer installer.
 * The customer setup only ACCEPTS a code. The plugin VERIFIES it
 * with the public key compiled into the binary.
 */

import { generateKeyPairSync, sign, verify, randomBytes, createHash } from "node:crypto";
import { existsSync, mkdirSync, readFileSync, writeFileSync, appendFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const ROOT = dirname(fileURLToPath(import.meta.url));
const KEY_DIR = process.env.WARLOCK_KEY_DIR ? resolve(process.env.WARLOCK_KEY_DIR) : ROOT;
const PRIVATE_PATH = join(KEY_DIR, "private.pem");
const PUBLIC_PATH = join(KEY_DIR, "public.pem");
const LEDGER_PATH = join(KEY_DIR, "issued-ledger.jsonl");

export const PRODUCTS = Object.freeze({
  MONOLITH: { id: "MONOLITH", model: "1666", name: "WARLOCK MONOLITH" },
  CHAINSAW: { id: "CHAINSAW", model: "1510", name: "WARLOCK CHAINSAW DISTORTION" },
});

export const EDITIONS = Object.freeze({
  demo: { token: "DEMO", seats: 1, defaultDays: 14 },
  lifetime: { token: "LIFE", seats: 2, defaultDays: 0 },
});

function die(message, code = 1) {
  process.stderr.write(`error: ${message}\n`);
  process.exit(code);
}

function parseArgs(argv) {
  const out = { _: [] };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a.startsWith("--")) {
      const key = a.slice(2);
      const next = argv[i + 1];
      if (!next || next.startsWith("--")) out[key] = true;
      else { out[key] = next; i += 1; }
    } else out._.push(a);
  }
  return out;
}

export function initKeys(dir = KEY_DIR) {
  mkdirSync(dir, { recursive: true });
  const priv = join(dir, "private.pem");
  const pub = join(dir, "public.pem");
  if (existsSync(priv) || existsSync(pub)) {
    return { created: false, privatePath: priv, publicPath: pub };
  }
  const { publicKey, privateKey } = generateKeyPairSync("ed25519");
  writeFileSync(priv, privateKey.export({ type: "pkcs8", format: "pem" }), { mode: 0o600 });
  writeFileSync(pub, publicKey.export({ type: "spki", format: "pem" }), { mode: 0o644 });
  return { created: true, privatePath: priv, publicPath: pub };
}

function loadPrivate() {
  if (!existsSync(PRIVATE_PATH)) die(`private key missing: ${PRIVATE_PATH}\nRun: node warlock-license.mjs init-keys`);
  return readFileSync(PRIVATE_PATH);
}

function loadPublic() {
  if (!existsSync(PUBLIC_PATH)) die(`public key missing: ${PUBLIC_PATH}\nRun: node warlock-license.mjs init-keys`);
  return readFileSync(PUBLIC_PATH);
}

function b64url(buf) { return Buffer.from(buf).toString("base64url"); }
function fromB64url(s) { return Buffer.from(s, "base64url"); }

export function makePayload({ product, edition, email, seats, days, issuedAt }) {
  const prod = PRODUCTS[product];
  if (!prod) throw new Error(`unknown product: ${product}`);
  const ed = EDITIONS[edition];
  if (!ed) throw new Error(`unknown edition: ${edition}`);
  const iat = issuedAt || Math.floor(Date.now() / 1000);
  const exp = days && Number(days) > 0 ? iat + Number(days) * 86400 : 0;
  const id = randomBytes(6).toString("hex").toUpperCase();
  return { v: 1, prd: prod.id, model: prod.model, ed: ed.token, seats: Number(seats || ed.seats), email: String(email || "").trim().toLowerCase(), iat, exp, id };
}

export function encodeKey(payload, privatePem) {
  const body = Buffer.from(JSON.stringify(payload), "utf8");
  const sig = sign(null, body, privatePem);
  return `WL1.${payload.prd}.${payload.ed}.${payload.id}.${b64url(body)}.${b64url(sig)}`;
}

export function decodeKey(keyText) {
  const key = String(keyText || "").trim().replace(/\s+/g, "");
  const parts = key.split(".");
  if (parts.length !== 6 || parts[0] !== "WL1") return { ok: false, error: "Malformed key. Expected WL1.PRODUCT.EDITION.ID.BODY.SIG" };
  const [, prd, ed, id, bodyB64, sigB64] = parts;
  let payload;
  try { payload = JSON.parse(fromB64url(bodyB64).toString("utf8")); }
  catch { return { ok: false, error: "Key body is not valid." }; }
  if (payload.prd !== prd || payload.ed !== ed || payload.id !== id) return { ok: false, error: "Key header does not match signed body." };
  return { ok: true, payload, body: fromB64url(bodyB64), sig: fromB64url(sigB64), key };
}

export function verifyKey(keyText, publicPem) {
  const decoded = decodeKey(keyText);
  if (!decoded.ok) return decoded;
  const good = verify(null, decoded.body, publicPem, decoded.sig);
  if (!good) return { ok: false, error: "Signature rejected. This code was not issued by Warlock Audio." };
  const now = Math.floor(Date.now() / 1000);
  if (decoded.payload.exp > 0 && now > decoded.payload.exp) return { ok: false, error: "This evaluation code has expired.", payload: decoded.payload };
  return { ok: true, payload: decoded.payload, key: decoded.key };
}

function appendLedger(record) {
  appendFileSync(LEDGER_PATH, `${JSON.stringify(record)}\n`, { encoding: "utf8" });
}

export function issue(options) {
  const payload = makePayload(options);
  const key = encodeKey(payload, loadPrivate());
  const record = { issued_at: new Date().toISOString(), product: payload.prd, edition: payload.ed, email: payload.email, seats: payload.seats, id: payload.id, exp: payload.exp, key };
  appendLedger(record);
  return record;
}

function printIssued(record) {
  const expText = record.exp ? new Date(record.exp * 1000).toISOString().slice(0, 10) : "NEVER (lifetime)";
  process.stdout.write([
    "---- WARLOCK LICENSE ----",
    `Product : ${record.product}`,
    `Edition : ${record.edition === "LIFE" ? "LIFETIME" : "DEMO"}`,
    `Email   : ${record.email || "(unbound)"}`,
    `Seats   : ${record.seats}`,
    `Id      : ${record.id}`,
    `Expires : ${expText}`,
    `Key     : ${record.key}`,
    "-------------------------",
    "Paste the Key line into the installer. Keep a copy on the receipt.",
    "",
  ].join("\n"));
}

function issueFromCsv(csvPath, edition, product) {
  const raw = readFileSync(csvPath, "utf8");
  const lines = raw.split(/\r?\n/).map((l) => l.trim()).filter(Boolean);
  const header = lines.shift();
  if (!header || !header.toLowerCase().includes("email")) die("CSV must start with a header containing an email column.");
  const cols = header.split(",").map((c) => c.trim().toLowerCase());
  const emailIdx = cols.indexOf("email");
  const nameIdx = cols.indexOf("name");
  const out = [];
  for (const line of lines) {
    const cells = line.split(",").map((c) => c.trim());
    const rec = issue({ product, edition, email: cells[emailIdx] });
    rec.name = nameIdx >= 0 ? cells[nameIdx] : "";
    printIssued(rec);
    out.push(rec);
  }
  const receipt = join(KEY_DIR, `receipts-${Date.now()}.json`);
  writeFileSync(receipt, JSON.stringify(out, null, 2));
  process.stdout.write(`Wrote ${out.length} receipts → ${receipt}\n`);
}

function cmdHelp() {
  process.stdout.write(`Warlock License Desk — factory issuer / verifier\n\nCommands\n  init-keys  issue  batch  verify  fingerprint\n`);
}

function fingerprint(pem) {
  const b = Buffer.from(String(pem).replace(/-----[^-]+-----/g, "").replace(/\s+/g, ""), "base64");
  return createHash("sha256").update(b).digest("hex").slice(0, 16).toUpperCase();
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const cmd = args._[0] || "help";
  if (cmd === "help" || cmd === "-h" || cmd === "--help") { cmdHelp(); return; }
  if (cmd === "init-keys") {
    const result = initKeys();
    if (!result.created) die(`keys already exist in ${KEY_DIR}`);
    process.stdout.write(`Created:\n  ${result.privatePath}\n  ${result.publicPath}\n`);
    process.stdout.write(`Public fingerprint: ${fingerprint(readFileSync(result.publicPath))}\n`);
    process.stdout.write("Store private.pem as GitHub secret WARLOCK_LICENSE_PRIVATE_KEY.\n");
    return;
  }
  if (cmd === "fingerprint") { process.stdout.write(`${fingerprint(loadPublic())}\n`); return; }
  if (cmd === "issue") {
    if (!args.edition) die("--edition demo|lifetime is required");
    printIssued(issue({ product: args.product || "MONOLITH", edition: String(args.edition).toLowerCase(), email: args.email || "", seats: args.seats, days: args.days }));
    return;
  }
  if (cmd === "batch") {
    if (!args.csv) die("--csv path is required");
    if (!args.edition) die("--edition demo|lifetime is required");
    issueFromCsv(args.csv, String(args.edition).toLowerCase(), args.product || "MONOLITH");
    return;
  }
  if (cmd === "verify") {
    const key = args._[1] || args.key;
    if (!key) die("pass the key as the next argument");
    const result = verifyKey(key, loadPublic());
    if (!result.ok) die(result.error);
    process.stdout.write(`${JSON.stringify({ ok: true, payload: result.payload }, null, 2)}\n`);
    return;
  }
  die(`unknown command: ${cmd}`);
}

const invoked = process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (invoked) main();
