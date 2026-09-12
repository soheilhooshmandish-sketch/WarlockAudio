import { Link } from "@tanstack/react-router";
import { Menu, X } from "lucide-react";
import { useState } from "react";

const navItems = [
  { to: "/products" as const, label: "Products" },
  { to: "/generate" as const, label: "Generate" },
  { to: "/presets" as const, label: "Presets" },
  { to: "/skins" as const, label: "Skins" },
  { to: "/pricing" as const, label: "Pricing" },
  { to: "/support" as const, label: "Support" },
];

type SiteHeaderProps = {
  section?: string;
  active?: (typeof navItems)[number]["to"];
  rightSlot?: React.ReactNode;
};

export function SiteHeader({ section = "PLUGINS", active, rightSlot }: SiteHeaderProps) {
  const [menuOpen, setMenuOpen] = useState(false);

  return (
    <header className="warlock-app-header warlock-shared-header">
      <Link to="/" className="warlock-app-brand" aria-label="WARLOCK home" onClick={() => setMenuOpen(false)}>
        <img src="/brand/warlock-core.svg" alt="" />
        <span>
          <strong>WARLOCK</strong>
          <small>{section}</small>
        </span>
      </Link>

      <nav className="warlock-desktop-nav" aria-label="Primary navigation">
        {navItems.map((item) => (
          <Link key={item.to} to={item.to} className={active === item.to ? "is-active" : ""}>
            {item.label}
          </Link>
        ))}
      </nav>

      <div className="warlock-header-actions">
        <span className="warlock-live-dot"><i /> LIVE</span>
        {rightSlot ?? <Link to="/generate" className="warlock-button warlock-button-primary">CREATE</Link>}
        <button
          className="warlock-menu-trigger"
          type="button"
          aria-expanded={menuOpen}
          aria-label={menuOpen ? "Close menu" : "Open menu"}
          onClick={() => setMenuOpen((value) => !value)}
        >
          {menuOpen ? <X size={20} /> : <Menu size={20} />}
        </button>
      </div>

      {menuOpen ? (
        <div className="warlock-mobile-menu">
          <div className="warlock-mobile-menu-head">
            <p>NAVIGATION</p>
            <span>SOUND BEYOND REALITY</span>
          </div>
          {navItems.map((item, index) => (
            <Link key={item.to} to={item.to} onClick={() => setMenuOpen(false)}>
              <span>{String(index + 1).padStart(2, "0")}</span>
              <strong>{item.label}</strong>
            </Link>
          ))}
          <Link to="/generate" className="warlock-mobile-create" onClick={() => setMenuOpen(false)}>
            CREATE WITH WARLOCK
          </Link>
        </div>
      ) : null}
    </header>
  );
}
