# WARLOCK WEBSITE — CURRENT STATE

Verified snapshot: 2026-09-12.

## Repository
- Repo: `soheilhooshmandish-sketch/WarlockAudio`
- `main` at this snapshot: `36decac580bb4382c4ffb9a4ef3a24b403b8e8e2`
- Always re-fetch `main` before editing; this SHA is historical evidence only.

## Quality gate
Website CI currently proves:
- strict locked dependency install
- immutable `package-lock.json`
- TypeScript
- platform/auth contracts
- Factory intake policy
- preset/skin contracts
- account contracts
- public-health fail-closed contracts
- sandbox-commerce lifecycle
- default website build
- Cloudflare Pages Nitro build
- Cloudflare worker output (`dist/_worker.js/index.js`, `dist/_routes.json`)
- local Cloudflare/Workerd HTTP smoke across primary public/account routes

Do not merge executable website changes unless current Website CI succeeds.

## Implemented public surfaces
- `/`
- `/generate`
- `/products`
- `/products/:slug`
- `/pricing`
- `/presets`
- `/skins`
- `/technology`
- `/support`
- `/status`
- `/login`
- `/account`
- `/account/products`
- `/account/builds`
- `/account/licenses`
- `/account/billing`

## Generator truth
The browser does not receive Factory secrets.
Public Factory intake stays fail-closed unless explicitly armed. Remote intake requires HTTPS, Cloudflare Access service credentials and the server-only WARLOCK intake token.
The UI may stage a request only; staging is not build/payment/license/release proof.

Website -> Factory intake remains blocked until Factory PR #64 is CI-proven/merged and the required preview secrets are configured.

## Product truth
- VOID = Guitar Test Development, not commercially available.
- Other Signature products remain concept/DNA unless backed by release evidence.
- Homepage/product pages use the same centralized catalog truth.

## Presets / Skins
Versioned, hashed compatibility contracts exist. Public commercial catalogs stay empty until real assets exist.
Custom skin upload processing is not live; unsafe/scriptable/external-reference content stays rejected.

## Account / Auth
Existing Better Auth integration is reused.
Account routes are real; customer product/build/license/billing datasets stay empty until account-bound production backends exist.
Ready product/build state requires exact artifact SHA256. License target remains perpetual, max two devices. Billing cannot fabricate a live plan.

## Public status
`/status` cannot claim live Factory health without telemetry. Unknown stays unknown.

## Sandbox commerce
A deterministic sandbox-only lifecycle is CI-tested:
`cart -> checkout -> simulated payment -> order -> entitlement -> test license -> exact artifact -> install -> device 1 -> device 2 -> revoke -> refund simulation`

It moves no real money, contacts no customer, issues no production license and grants no production download.

## Cloudflare deployment truth
Cloudflare is the selected deployment path.

Proven:
- Nitro `cloudflare_pages` target builds successfully.
- Cloudflare worker/functions output is present.
- Wrangler/Workerd runs the generated output locally.
- Primary routes pass HTTP runtime smoke.
- dedicated `preview/cloudflare` branch trigger exists.
- workflow permits only a Pages project ending in `-preview`.
- Factory public intake is DISARMED in preview.
- production-domain promotion is not part of the preview workflow.

Not yet proven:
- a live Cloudflare preview URL.

The first real preview run (`34685850659`) stopped before deployment because both GitHub preview credentials were empty:
- `CLOUDFLARE_API_TOKEN`
- `CLOUDFLARE_ACCOUNT_ID`

Issue #23 tracks this one-time credential setup. No Cloudflare project/domain/deployment was changed by the failed run.

Committed `.vercel/output` is legacy generated material and is not deployment proof. `.vercel/` is ignored going forward.

## Governance blockers
- Website `main` remains unprotected.
- Issue #7 tracks branch protection before launch.

## Factory dependency
Factory private GitHub Actions still fail before Step 1 (issue #65). Factory PR #64 must not merge until private CI executes real steps.

## Not live / not proven
- live Cloudflare preview (blocked only by issue #23 credentials)
- production Factory intake end-to-end
- Prompt Refiner backend
- live billing/checkout
- production entitlements/licenses/activation
- customer ticket/email delivery
- live public Factory telemetry
- commercial preset/skin catalog
- branch protection
- production launch
