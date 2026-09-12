# WARLOCK WEBSITE — CURRENT STATE

Verified snapshot: 2026-09-12.

## Repository
- Repo: `soheilhooshmandish-sketch/WarlockAudio`
- `main` at this snapshot: `a396aee8c82b83b9d3c3cf12e7a59b1158b852a8`
- Always re-fetch `main` before editing; this SHA is historical evidence only.

## Quality gate
Website CI currently requires:
- strict `npm ci --ignore-scripts --no-audit --no-fund`
- `package-lock.json` remains immutable
- TypeScript passes
- platform/auth contracts pass
- Factory intake policy tests pass
- preset/skin asset contracts pass
- account contracts pass
- public-health fail-closed contracts pass
- sandbox-commerce lifecycle tests pass
- production build passes

Do not merge executable website changes unless the relevant current Website CI succeeds.

## Implemented public surfaces
- `/` company homepage
- `/generate` staged Generator workspace
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
The website has a server-side Factory intake bridge with a fail-closed policy:
- public intake must be explicitly armed
- remote gateway must use HTTPS
- remote gateway requires Cloudflare Access service credentials
- WARLOCK intake token is server-only
- missing/invalid configuration blocks the request

The UI may stage a request only. It must not claim that staging means build, payment, license or release.

End-to-end Website -> Factory intake is not complete until Factory PR #64 is CI-proven/merged and deployment secrets are configured.

## Product truth
- VOID is presented as Guitar Test Development, not commercially available.
- Other Signature names remain concept/DNA unless backed by real release evidence.
- Product catalog data is centralized so homepage/detail pages do not invent separate statuses.

## Presets / Skins
Typed versioned contracts exist.
Public commercial catalogs intentionally remain empty until real assets have:
- compatibility
- minimum product version
- schema/brand-geometry version
- exact content SHA256

Custom skin upload processing is not live. SVG/executable/external-reference content is fail-closed until a dedicated sanitizer exists.

## Account / Auth
Existing Better Auth integration is reused.
Account UI is real, but customer product/build/license/billing data is intentionally empty until real account-bound backends exist.

Fail-closed invariants:
- ready product requires exact artifact SHA256
- ready build requires exact artifact SHA256
- license target is perpetual, max two devices
- disconnected billing cannot display invented plan, balance or renewal data

## Public status
`/status` does not claim live Factory health.
Live telemetry is currently disconnected, so overall HEALTHY is forbidden by contract.
Unknown stays unknown; website build proof is distinguished from live uptime proof.

## Sandbox commerce
A deterministic sandbox-only lifecycle exists and is tested:
`cart -> checkout -> simulated payment -> order -> entitlement -> test license -> exact artifact -> install -> device 1 -> device 2 -> revoke -> refund simulation`

It has no live payment-provider call, moves no real money, contacts no customer, issues no production license and grants no production download.

## Deployment truth
No GitHub deployment workflow was found for this website.
`.vercel/output` exists as committed generated output, but there is no `.vercel/project.json` in the repository and this is **not** proof of a live preview deployment.
A Vercel connector is available but must be explicitly connected before ChatGPT can inspect/create deployment there.

Do not claim preview or production deployment until a live URL and deployment state are actually verified.

## Governance blockers
- Website `main` is unprotected in current GitHub metadata.
- Issue #7 tracks branch protection before launch.

## Factory dependency
Factory repo currently has a private GitHub Actions infrastructure blocker (issue #65). PR #64 for secure website intake must not merge until private CI receives a real runner and executes normally.

## Not live / not proven
- production Factory intake end-to-end
- Prompt Refiner backend (Refine/Enhance/Pro intelligence)
- live billing/checkout
- production entitlements/licenses/activation
- customer ticket/email delivery
- live public Factory telemetry
- commercial preset/skin catalog
- verified preview/production deployment
- branch protection

The website architecture is substantially built, but these unavailable capabilities must remain visibly locked rather than simulated.
