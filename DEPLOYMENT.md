# WARLOCK Website Deployment Gate

Production deployment is never inferred from committed build output.

## Deployment target

WARLOCK keeps its public deployment on Cloudflare.

For the current TanStack Start + Nitro stack, preview deployment uses Nitro's `cloudflare_pages` target so SSR/server functions remain available. The custom production domain stays untouched during preview work.

## Preview first

A real Cloudflare preview must exist before production promotion.

Required preview proof:

1. preview URL exists;
2. deployed commit SHA is recorded;
3. environment is explicitly preview/non-production;
4. Factory public intake remains disarmed by default;
5. live billing remains off;
6. all public routes render successfully;
7. account/login behavior is checked with the preview auth configuration;
8. no browser bundle contains Factory secrets;
9. mobile/desktop/keyboard/reduced-motion checks pass;
10. Founder reviews the preview before any production-domain change.

Cloudflare preview workflow must never bind `warlock-plugins.com` automatically.

## Production

Do not promote to production until the launch blockers in `WEBSITE_NEXT.md` are resolved and Founder approval is explicit.

Committed `.vercel/output` is legacy generated build material, not proof of a deployed website. `.vercel/` is ignored going forward.
