#!/usr/bin/env node
import { createServer } from "node:http";
import { issue, verifyKey, PRODUCTS, EDITIONS } from "./warlock-license.mjs";
import { existsSync, readFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const ROOT = dirname(fileURLToPath(import.meta.url));
const PUBLIC = readFileSync(join(ROOT, "desk.html"), "utf8");
const PORT = Number(process.env.WARLOCK_DESK_PORT || 1666);

if (!existsSync(join(ROOT, "private.pem"))) {
  process.stderr.write("Run first: node warlock-license.mjs init-keys\n");
  process.exit(1);
}

const server = createServer(async (req, res) => {
  const url = new URL(req.url, "http://127.0.0.1");
  const local = req.socket.remoteAddress === "127.0.0.1" || req.socket.remoteAddress === "::1";
  if (!local) { res.writeHead(403); res.end("Factory desk is localhost only."); return; }
  if (req.method === "GET" && url.pathname === "/") {
    res.writeHead(200, { "content-type": "text/html; charset=utf-8" });
    res.end(PUBLIC); return;
  }
  if (req.method === "POST" && url.pathname === "/issue") {
    const body = await readBody(req);
    try {
      const rec = issue({ product: body.product || "MONOLITH", edition: body.edition || "lifetime", email: body.email || "", seats: body.seats, days: body.days });
      res.writeHead(200, { "content-type": "application/json" });
      res.end(JSON.stringify(rec));
    } catch (err) {
      res.writeHead(400, { "content-type": "application/json" });
      res.end(JSON.stringify({ error: String(err.message || err) }));
    }
    return;
  }
  if (req.method === "POST" && url.pathname === "/verify") {
    const body = await readBody(req);
    const result = verifyKey(body.key || "", readFileSync(join(ROOT, "public.pem")));
    res.writeHead(result.ok ? 200 : 400, { "content-type": "application/json" });
    res.end(JSON.stringify(result));
    return;
  }
  res.writeHead(404); res.end("not found");
});

function readBody(req) {
  return new Promise((resolve, reject) => {
    const chunks = [];
    req.on("data", (c) => chunks.push(c));
    req.on("end", () => { try { resolve(JSON.parse(Buffer.concat(chunks).toString("utf8") || "{}")); } catch (err) { reject(err); } });
    req.on("error", reject);
  });
}

server.listen(PORT, "127.0.0.1", () => {
  process.stdout.write(`Warlock License Desk  http://127.0.0.1:${PORT}\n`);
  process.stdout.write(`Products: ${Object.keys(PRODUCTS).join(", ")}  Editions: ${Object.keys(EDITIONS).join(", ")}\n`);
});
