# WARLOCK WEBSITE — CURRENT STATE

Verified snapshot: 2026-09-12.

## Repository
- Repo: `soheilhooshmandish-sketch/WarlockAudio`
- Always re-fetch `main` before editing; recorded SHAs are evidence snapshots, not permanent truth.

## Quality gate
Website CI currently proves:
- strict locked dependency install
- immutable `package-lock.json`
- TypeScript
- platform/auth contracts
- Factory intake policy
- FARNAZ Prompt Refiner contract
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

Website -> Factory intake remains blocked until Factory PR #64 is CI-proven/merged and the required controlled deployment configuration exists.

## FARNAZ Prompt Refiner
A fail-closed typed contract exists for REFINE / ENHANCE / PRO.
It preserves Original Prompt and protected technical facts, requires explicit proposals for protected-value changes, and forbids browser-side provider calls or silent auto-application while the real FARNAZ service is disconnected.

No public AI rewrite service is currently claimed live.

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
Canonical deployment uses the **existing Cloudflare native Git integration**.

Confirmed deployment identity:
- Cloudflare project/site: `warlock-plugins-com`
- public domain: `warlock-plugins.com`
- Git source: `soheilhooshmandish-sketch/WarlockAudio`

The temporary GitHub Actions -> Cloudflare API-token deploy path is not canonical and is being removed to avoid two competing deployment systems.

Proven independently in Website CI:
- Nitro `cloudflare_pages` target builds successfully.
- Cloudflare worker/functions output is present.
- Wrangler/Workerd runs the generated output locally.
- Primary routes pass HTTP runtime smoke.

Not yet proven in this repository record:
- exact live preview URL for the current reviewed commit through the native Cloudflare Git integration.

The dedicated `preview/cloudflare` branch exists and can be used as a non-production Git trigger if the Cloudflare project is configured for branch preview deployments.

No GitHub `CLOUDFLARE_API_TOKEN` / `CLOUDFLARE_ACCOUNT_ID` secrets are required merely to use the already-connected native Git integration.

Committed `.vercel/output` is legacy generated material and is not deployment proof. `.vercel/` is ignored going forward.

## Governance blockers
- Website `main` remains unprotected.
- Issue #7 tracks branch protection before launch.

## Factory dependency
Factory private GitHub Actions still fail before Step 1 (issue #65). Factory PR #64 must not merge until private CI executes real steps.

## Not live / not proven
- current live Cloudflare preview proof for reviewed commit
- production Factory intake end-to-end
- live FARNAZ Prompt Refiner backend
- live billing/checkout
- production entitlements/licenses/activation
- customer ticket/email delivery
- live public Factory telemetry
- commercial preset/skin catalog
- branch protection
- production launch
