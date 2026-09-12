import { createFileRoute, Link } from "@tanstack/react-router";
import {
  Activity,
  ArrowLeft,
  BadgeEuro,
  ChevronDown,
  CircleHelp,
  Headphones,
  LifeBuoy,
  Mail,
  MessageSquareText,
  Send,
  ShieldAlert,
} from "lucide-react";
import { useState } from "react";
import { SiteHeader } from "@/components/warlock/site-header";

export const Route = createFileRoute("/support")({ component: SupportPage });

const faq = [
  ["Does refining a prompt use a build?", "No. Prompt refinement and non-build preview are intended to remain outside the paid build allowance."],
  ["Does a Generator subscription make my plugin subscription-only?", "No. Generator access and finished product licensing are separate. Finished products follow their own license terms."],
  ["How many devices can a finished WARLOCK product use?", "The current target policy is a perpetual product license with up to two activated devices, where supported."],
  ["What happens if a build fails validation?", "The build should stop at Production Health, preserve the project state and provide a useful retry path instead of pretending the build succeeded."],
];

function SupportPage() {
  const [openFaq, setOpenFaq] = useState(0);
  const [message, setMessage] = useState("");

  return (
    <main className="warlock-app-page warlock-page-enter">
      <SiteHeader section="SUPPORT" active="/support" />

      <section className="warlock-simple-hero warlock-support-hero">
        <Link to="/" className="warlock-backlink"><ArrowLeft size={15} /> BACK TO WARLOCK</Link>
        <p className="warlock-app-kicker">SUPPORT & CONTACT</p>
        <h1>GET THE RIGHT HELP.<br /><span>WITHOUT THE RUNAROUND.</span></h1>
        <p>Installation, licensing, Factory builds, sales and technical support should each have a clear route.</p>
      </section>

      <section className="warlock-support-channel-grid">
        {[
          ["TECHNICAL SUPPORT", "Installation, activation, host behavior and troubleshooting.", Headphones],
          ["SALES & LICENSING", "Plans, product licensing, partnerships and commercial questions.", BadgeEuro],
          ["BUILD PROBLEM", "Report a failed or incorrect Factory build with project context.", ShieldAlert],
          ["SYSTEM STATUS", "Check service state before opening a support request.", Activity],
          ["GENERAL CONTACT", "Company, press and non-technical enquiries.", Mail],
          ["FAQ", "Fast answers for common product and account questions.", CircleHelp],
        ].map(([title, body, Icon]) => {
          const IconComponent = Icon as typeof Headphones;
          return (
            <button key={String(title)} className="warlock-support-channel">
              <span><IconComponent size={22} /></span>
              <div><strong>{String(title)}</strong><small>{String(body)}</small></div>
              <LifeBuoy size={17} />
            </button>
          );
        })}
      </section>

      <section className="warlock-contact-layout">
        <div className="warlock-contact-form-wrap">
          <div className="warlock-generator-copy">
            <p className="warlock-app-kicker">CONTACT WARLOCK</p>
            <h2>Write the problem once.</h2>
            <p>This preview keeps the form local because the support backend is not yet connected. It must not pretend a ticket was sent.</p>
          </div>
          <div className="warlock-contact-form">
            <label><span>TOPIC</span><select defaultValue="technical"><option value="technical">Technical Support</option><option value="build">Build Problem</option><option value="sales">Sales & Licensing</option><option value="general">General Contact</option></select></label>
            <div className="warlock-contact-two"><label><span>NAME</span><input placeholder="Your name" /></label><label><span>EMAIL</span><input type="email" placeholder="you@example.com" /></label></div>
            <label><span>MESSAGE</span><textarea value={message} onChange={(event) => setMessage(event.target.value)} placeholder="Tell us what happened, what you expected, and any relevant product/build information..." /></label>
            <button className="warlock-button warlock-button-primary" disabled><Send size={16} /> SUPPORT BACKEND NOT CONNECTED</button>
          </div>
        </div>

        <aside className="warlock-support-side">
          <article><MessageSquareText size={20} /><div><strong>MY SUPPORT REQUESTS</strong><span>Authenticated ticket history will live inside Account when the support backend is connected.</span></div></article>
          <article><Activity size={20} /><div><strong>BUILD EVIDENCE</strong><span>Factory support should attach exact ProductSpec, build and validation evidence automatically.</span></div></article>
          <article><ShieldAlert size={20} /><div><strong>NO RAW STACK TRACES</strong><span>Customers receive specific, useful error explanations. Engineering logs stay internal.</span></div></article>
        </aside>
      </section>

      <section className="warlock-faq-section">
        <div className="warlock-section-heading"><p>FAQ</p><h2>COMMON QUESTIONS</h2></div>
        <div className="warlock-faq-list">
          {faq.map(([question, answer], index) => (
            <button key={question} className={openFaq === index ? "is-open" : ""} onClick={() => setOpenFaq(openFaq === index ? -1 : index)}>
              <div><strong>{question}</strong><ChevronDown size={18} /></div>
              {openFaq === index ? <p>{answer}</p> : null}
            </button>
          ))}
        </div>
      </section>
    </main>
  );
}
