# WARLOCK Website Deployment Gate

Production deployment is not assumed from committed build output.

## Preview first

A real preview deployment must be created and inspected before production. Preferred current path: Vercel connector after the Founder explicitly connects the project/account.

Required preview proof:

1. deployment URL exists;
2. deployed commit SHA is recorded;
3. environment is preview/non-production;
4. Factory public intake remains disarmed by default;
5. live billing remains off;
6. all public routes render successfully;
7. account/login behavior is checked with the actual deployment auth configuration;
8. no browser bundle contains Factory secrets;
9. mobile/desktop/keyboard/reduced-motion checks pass;
10. Founder reviews the preview before any production deployment.

## Production

Do not promote to production until the launch blockers in `WEBSITE_NEXT.md` are resolved and Founder approval is explicit.

Committed `.vercel/output` is generated build material, not proof of a deployed website.
