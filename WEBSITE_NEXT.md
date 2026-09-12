# WARLOCK WEBSITE — NEXT

Read `WEBSITE_CURRENT_STATE.md` before implementing new public features.

## P0 — Deployment preview
1. Connect an approved deployment provider (Vercel connector is available) or explicitly choose the WARLOCK Cloudflare Pages deployment path.
2. Create a **preview**, not production deployment.
3. Keep live billing off and Factory public intake disarmed by default.
4. Verify preview URL, build SHA and environment mode.
5. Test routes on real preview:
   - `/`
   - `/generate`
   - `/products`
   - `/products/void`
   - `/pricing`
   - `/presets`
   - `/skins`
   - `/technology`
   - `/support`
   - `/status`
   - `/login`
   - `/account`
6. Verify mobile/desktop layout, keyboard focus, reduced-motion behavior and no horizontal overflow.
7. Founder reviews preview before production deployment.

Do not interpret committed `.vercel/output` as deployment proof.

## P1 — Factory intake end-to-end
Blocked until Factory private Actions issue #65 is resolved and Factory PR #64 is CI-proven/merged.

After that:
- configure preview-only server secrets
- keep browser secret-free
- confirm Cloudflare Access + WARLOCK intake token both enforce auth
- arm `WARLOCK_FACTORY_PUBLIC_INTAKE_ENABLED=true` only for controlled test
- submit harmless Generator prompt
- verify exact staged request UUID and fail-closed truth flags
- test idempotent replay and conflict rejection
- disarm again unless preview operations require it

## P2 — Prompt Refiner
Do not fake AI text transformations in the browser.
Connect Generator Review/Refine to the proven FARNAZ/Factory service after it exists.
Required modes:
- REFINE
- ENHANCE
- PRO

Always show Original, Refined and Suggestions separately. Expert technical values must not be silently changed.

## P3 — Account-bound jobs
After authenticated Factory intake exists:
- bind request/job to account identity
- expose only that account's build states
- show exact artifact identity when ready
- preserve staged/queued/building/validating/packaging/ready/failed/blocked/cancelled states
- do not expose private CI logs

## P4 — Provider sandbox commerce
The deterministic sandbox state machine already exists.
Next, integrate a payment provider **sandbox** only:
- test checkout
- sandbox payment result
- order persistence
- entitlement
- test license
- test download
- activation device 1 and 2
- revoke/reissue
- refund/cancel simulation

No live credentials or real charges until explicit Founder approval after the full sandbox report passes.

## P5 — Live health telemetry
`/status` currently fails closed.
Connect only an authenticated/redacted Gateway/FARNAZ health summary.
Never send Factory secrets, customer data, license keys or raw private logs to the public site.

## P6 — Customer support backend
Only after monitored delivery exists:
- authenticated support request
- exact product/build/version/artifact context
- safe diagnostic attachment metadata
- case state
- optional approved email/human workflow

Do not promise response times until operations can support them.

## P7 — Real preset/skin assets
Add catalog entries only when real assets exist and exact compatibility/hash metadata is known.
No placeholder commercial inventory.

## P8 — Governance
Enable website `main` branch protection (issue #7):
- PR required
- Website CI required
- conversation resolution
- block force-push/delete
- documented break-glass behavior

## P9 — Production launch
Before production:
- preview approved
- branch protection enabled
- real auth tested
- Factory intake tested and intentionally armed/disarmed as launch design requires
- product availability matches Factory release evidence
- legal/tax/KVK/VAT/payment decisions complete
- licensing enforcement tested end to end
- support path operational
- production secrets configured outside Git
- Founder explicitly approves deployment

Do not deploy experimental or partially-connected behavior as if commercial launch were complete.
