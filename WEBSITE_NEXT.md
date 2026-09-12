# WARLOCK WEBSITE — NEXT

Read `WEBSITE_CURRENT_STATE.md` before implementing new public features.

## P0 — Finish live Cloudflare preview
Already proven in CI:
- target-selectable Nitro build
- `cloudflare_pages` output
- Workerd runtime
- primary-route HTTP smoke
- isolated `preview/cloudflare` trigger
- preview-only `*-preview` project guard
- Factory intake disarmed

Current blocker: issue #23.
One-time GitHub `preview` environment/repository secrets required:
- `CLOUDFLARE_API_TOKEN`
- `CLOUDFLARE_ACCOUNT_ID`

Do not put secret values in source, issues or chat logs.

After credentials are configured:
1. push/update `preview/cloudflare`;
2. confirm the workflow creates/uses only `warlock-audio-preview` (or another explicit `*-preview` project);
3. capture the live preview URL and deployed commit SHA;
4. verify all primary routes on the live URL;
5. check mobile/desktop layout, keyboard focus, reduced-motion and horizontal overflow;
6. Founder reviews preview;
7. do not promote the production domain yet.

## P1 — Factory intake end-to-end
Blocked until Factory private Actions issue #65 is resolved and Factory PR #64 is CI-proven/merged.
Then configure preview-only server secrets, keep browser secret-free, test idempotent website staging, and disarm again after controlled testing unless launch design requires otherwise.

## P2 — Prompt Refiner
Do not fake AI transformations in the browser.
Connect Review/Refine only to a proven FARNAZ/Factory service. Required modes: REFINE, ENHANCE, PRO. Always show Original, Refined and Suggestions separately.

## P3 — Account-bound jobs
After authenticated Factory intake exists, bind jobs to account identity and expose only that account's states/artifact identity. Do not expose private CI logs.

## P4 — Provider sandbox commerce
The deterministic sandbox lifecycle exists. Next integrate a payment provider sandbox only: checkout, sandbox payment, order, entitlement, test license/download, two-device activation, revoke/reissue and refund/cancel simulation.
No real credentials/charges until explicit Founder approval after the sandbox report passes.

## P5 — Live health telemetry
Connect `/status` only to authenticated/redacted Gateway/FARNAZ health summary. Never expose Factory secrets, customer data, license keys or raw private logs.

## P6 — Customer support backend
Only after monitored delivery exists: authenticated support request, exact product/build/artifact context, safe diagnostic metadata and case state.

## P7 — Real preset/skin assets
Add catalog entries only with real compatibility/version/hash metadata. No placeholder commercial inventory.

## P8 — Governance
Enable website `main` branch protection (issue #7): PR required, Website CI required, conversation resolution, block force-push/delete, documented break-glass behavior.

## P9 — Production launch
Requires approved live Cloudflare preview, branch protection, tested real auth, proven Factory intake, evidence-backed product availability, legal/tax/KVK/VAT/payment decisions, tested licensing, operational support, production secrets outside Git and explicit Founder approval.

Do not deploy partially-connected behavior as a commercial launch.
