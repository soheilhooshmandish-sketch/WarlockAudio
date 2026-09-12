import { Link } from "@tanstack/react-router";
import {
  Box,
  CircleUserRound,
  CreditCard,
  Hammer,
  KeyRound,
  LockKeyhole,
  ShieldCheck,
  type LucideIcon,
} from "lucide-react";
import { RedirectToSignIn } from "@/lib/auth/gates";
import { useCurrentUserState } from "@/lib/auth/use-current-user";
import {
  accountBuilds,
  accountLicenses,
  accountProducts,
  billingSummary,
} from "@/lib/account/model";
import "../../warlock-account.css";

export type AccountSection = "overview" | "products" | "builds" | "licenses" | "billing";
type AccountPath =
  | "/account"
  | "/account/products"
  | "/account/builds"
  | "/account/licenses"
  | "/account/billing";

const nav: Array<{ section: AccountSection; label: string; to: AccountPath }> = [
  { section: "overview", label: "OVERVIEW", to: "/account" },
  { section: "products", label: "MY PRODUCTS", to: "/account/products" },
  { section: "builds", label: "MY BUILDS", to: "/account/builds" },
  { section: "licenses", label: "LICENSES", to: "/account/licenses" },
  { section: "billing", label: "BILLING", to: "/account/billing" },
];

function EmptyState({ icon: Icon, title, copy }: { icon: LucideIcon; title: string; copy: string }) {
  return (
    <div className="wa-account-empty">
      <Icon size={25} aria-hidden="true" />
      <h2>{title}</h2>
      <p>{copy}</p>
    </div>
  );
}

function SectionContent({ section }: { section: AccountSection }) {
  if (section === "products" && accountProducts.length === 0) {
    return <EmptyState icon={Box} title="No account-bound products yet." copy="A product will appear here only after a real account entitlement is connected to an exact product/version artifact." />;
  }

  if (section === "builds" && accountBuilds.length === 0) {
    return <EmptyState icon={Hammer} title="No account-bound builds yet." copy="Factory intake is being connected, but staged requests are not fabricated into customer build history." />;
  }

  if (section === "licenses" && accountLicenses.length === 0) {
    return <EmptyState icon={KeyRound} title="No licenses issued yet." copy="The account UI will show activation and device state only after real license issuance and enforcement are connected." />;
  }

  if (section === "billing") {
    return (
      <div className="wa-account-billing">
        <CreditCard size={25} aria-hidden="true" />
        <p>BILLING CONNECTION</p>
        <h2>{billingSummary.liveBillingConnected ? "Connected" : "Not connected."}</h2>
        <span>
          No live plan, renewal date, build balance or payment method is invented before a production billing provider is explicitly connected and sandbox-tested.
        </span>
      </div>
    );
  }

  return (
    <div className="wa-account-overview-grid">
      <article><Box size={20} /><p>PRODUCTS</p><strong>{accountProducts.length}</strong><span>Real entitlements only</span></article>
      <article><Hammer size={20} /><p>BUILDS</p><strong>{accountBuilds.length}</strong><span>Account-bound jobs only</span></article>
      <article><KeyRound size={20} /><p>LICENSES</p><strong>{accountLicenses.length}</strong><span>Issued licenses only</span></article>
      <article><ShieldCheck size={20} /><p>BILLING</p><strong>{billingSummary.liveBillingConnected ? "LIVE" : "LOCKED"}</strong><span>No fake plan state</span></article>
    </div>
  );
}

export function AccountShell({ section }: { section: AccountSection }) {
  const { user, isPending } = useCurrentUserState();

  if (isPending) {
    return (
      <main className="wa-account-page">
        <div className="wa-account-loading">CHECKING ACCOUNT SESSION…</div>
      </main>
    );
  }

  if (!user) return <RedirectToSignIn />;

  const label = user.displayName ?? user.primaryEmail ?? "WARLOCK USER";

  return (
    <main className="wa-account-page">
      <header className="wa-account-topbar">
        <Link to="/" className="wa-account-brand"><span aria-hidden="true">W</span>WARLOCK</Link>
        <div className="wa-account-identity">
          <CircleUserRound size={18} aria-hidden="true" />
          <span>{label}</span>
          {user.isDevFallback ? <em>DEV AUTH MODE</em> : null}
        </div>
      </header>

      <div className="wa-account-layout">
        <aside>
          <p>ACCOUNT</p>
          <nav aria-label="Account navigation">
            {nav.map((item) => (
              <Link
                key={item.section}
                to={item.to}
                data-active={section === item.section ? "true" : undefined}
              >
                {item.label}
              </Link>
            ))}
          </nav>
          <Link to="/support" className="wa-account-support">SUPPORT</Link>
        </aside>

        <section className="wa-account-content">
          <div className="wa-account-heading">
            <p>{section === "overview" ? "MY WARLOCK" : nav.find((item) => item.section === section)?.label}</p>
            <h1>{section === "overview" ? "Account control without invented data." : section === "products" ? "My Products" : section === "builds" ? "My Builds" : section === "licenses" ? "Licenses" : "Billing"}</h1>
            <span>
              Customer data appears only when it is bound to the authenticated account and backed by the corresponding production system.
            </span>
          </div>
          <SectionContent section={section} />
        </section>
      </div>

      <footer className="wa-account-footer">
        <LockKeyhole size={16} aria-hidden="true" />
        <span>Account UI cannot issue licenses, move money or publish releases by itself.</span>
      </footer>
    </main>
  );
}
