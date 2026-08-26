import { createFileRoute, Link } from "@tanstack/react-router";
import { Download } from "lucide-react";
import type { ReactNode } from "react";

export const Route = createFileRoute("/howto")({ component: HowToPage });

const SHOTS = [
  { src: "/howto/01-run-as-admin.jpg", cap: "1. Run the installer as administrator" },
  { src: "/howto/02-welcome.jpg", cap: "2. Welcome — 64-bit VST3" },
  { src: "/howto/03-select-folder.jpg", cap: "3. Folder must end in .vst3" },
  { src: "/howto/04-installing.jpg", cap: "4. Installing" },
  { src: "/howto/05-finished.jpg", cap: "5. Finish" },
  { src: "/howto/06-bundle-folder.jpg", cap: "6. The bundle is a folder, not a lone DLL" },
  { src: "/howto/07-daw-rescan.jpg", cap: "7. Rescan in the DAW" },
];

function HowToPage() {
  return (
    <main className="page-bg min-h-dvh px-4 py-8 sm:px-8">
      <article className="mx-auto max-w-3xl border border-line bg-iron/80 p-5 sm:p-8">
        <p className="font-display text-xs tracking-[0.42em] text-blood uppercase">
          Warlock Audio · VST3 1.1.0
        </p>
        <h1 className="font-deco mt-2 text-3xl text-bone sm:text-4xl">
          Pack and install
        </h1>
        <p className="font-body mt-3 text-bone-dim">
          Skins, knobs, stomp, logo, and three cabinet IRs compile into the
          plugin. The DAW never hunts for loose files.
        </p>
        <div className="mt-5 flex flex-wrap gap-2">
          <a
            href="/Warlock-VST3-PC-Export.zip"
            download
            className="inline-flex min-h-11 items-center gap-2 border border-blood bg-blood/20 px-4 font-display text-xs tracking-[0.16em] text-bone uppercase"
          >
            <Download className="size-3.5" />
            Download build kit
          </a>
          <a
            href="/HOWTO.md"
            download
            className="inline-flex min-h-11 items-center border border-line px-4 font-display text-xs tracking-[0.16em] text-frost uppercase"
          >
            HOWTO.md
          </a>
          <Link
            to="/"
            className="inline-flex min-h-11 items-center border border-line px-4 font-display text-xs tracking-[0.16em] text-bone uppercase"
          >
            Back to pedal
          </Link>
        </div>

        <Section n="1" title="What gets packed">
          <p className="font-body text-sm text-bone-dim">
            Two layers. The DLL holds the UI. The Resources folder holds only
            what Cubase reads without loading the plugin.
          </p>
          <h3 className="font-display mt-4 text-xs tracking-[0.2em] text-frost uppercase">
            Inside the DLL (BinaryData)
          </h3>
          <Table
            rows={[
              ["chassis.jpg", "Pedal metal"],
              ["knob.jpg", "Aluminum knob"],
              ["stomp.jpg", "3PDT cap"],
              ["logo.png", "Warlock mark"],
              ["Cab_Center.wav", "4×12 on-axis"],
              ["Cab_Edge.wav", "4×12 off-axis"],
              ["CabinetIR.wav", "Bedroom cab"],
              ["legal.txt", "License"],
            ]}
          />
          <h3 className="font-display mt-4 text-xs tracking-[0.2em] text-frost uppercase">
            Beside the DLL
          </h3>
          <Table
            rows={[
              ["Snapshots/*.png", "Cubase Media Rack"],
              ["moduleinfo.json", "VST3 catalog"],
              ["legal.txt", "Host about box"],
              ["Plugin.ico", "Explorer icon"],
            ]}
          />
          <pre className="mt-4 overflow-x-auto border border-line bg-void p-3 font-mono text-[11px] leading-relaxed text-frost">
            {`C:\\Program Files\\Common Files\\VST3\\Warlock.vst3\\
├── desktop.ini
├── Plugin.ico
└── Contents\\
    ├── x86_64-win\\Warlock.vst3     ← DLL + all skins
    └── Resources\\
        ├── legal.txt
        ├── moduleinfo.json
        └── Snapshots\\`}
          </pre>
        </Section>

        <Section n="2" title="Tools">
          <ul className="font-body list-disc space-y-1 pl-5 text-sm text-bone-dim">
            <li>Windows 10/11 64-bit</li>
            <li>Visual Studio 2022 — Desktop development with C++</li>
            <li>CMake 3.22+ on PATH</li>
            <li>Git (CMake fetches JUCE 7.0.12)</li>
            <li>Optional: Inno Setup 6.3+ for the Setup.exe</li>
          </ul>
        </Section>

        <Section n="3" title="Pack the VST3">
          <ol className="font-body list-decimal space-y-2 pl-5 text-sm text-bone-dim">
            <li>Unzip the kit so you see CMakeLists.txt, build.bat, Source, Resources.</li>
            <li>
              Double-click <span className="text-bone">build.bat</span>, or in the
              VS x64 Native Tools prompt run{" "}
              <span className="text-bone">cmake --preset vs2022-x64</span> then{" "}
              <span className="text-bone">cmake --build --preset warlock-release</span>.
            </li>
            <li>
              Output:{" "}
              <span className="text-bone">
                build\Warlock_artefacts\Release\VST3\Warlock.vst3\
              </span>
            </li>
          </ol>
          <p className="font-body mt-3 text-sm text-bone-dim">
            Replace a skin or IR under Resources\ with the same filename, then
            run build.bat again. BinaryData is regenerated from those files.
          </p>
        </Section>

        <Section n="4" title="Pack the installer">
          <ol className="font-body list-decimal space-y-2 pl-5 text-sm text-bone-dim">
            <li>Confirm the inner DLL exists under Contents\x86_64-win\.</li>
            <li>
              Open <span className="text-bone">installer.iss</span> in Inno Setup
              → Build → Compile.
            </li>
            <li>
              Output:{" "}
              <span className="text-bone">
                BuildInstaller\Warlock_Distortion_Setup_x64.exe
              </span>
            </li>
          </ol>
          <pre className="mt-3 overflow-x-auto border border-line bg-void p-3 font-mono text-[11px] text-frost">
            Warlock_Distortion_Setup_x64.exe /VERYSILENT /NORESTART /ALLUSERS
          </pre>
        </Section>

        <Section n="5" title="Install">
          <p className="font-body text-sm text-frost">Path A — Setup.exe</p>
          <ol className="font-body mt-2 list-decimal space-y-1 pl-5 text-sm text-bone-dim">
            <li>Right-click the Setup → Run as administrator.</li>
            <li>
              Leave{" "}
              <span className="text-bone">
                C:\Program Files\Common Files\VST3\Warlock.vst3
              </span>
            </li>
            <li>Finish. Close the DAW if Windows asks.</li>
          </ol>
          <p className="font-body mt-4 text-sm text-frost">Path B — copy the folder</p>
          <pre className="mt-2 overflow-x-auto border border-line bg-void p-3 font-mono text-[11px] text-frost">
            {`xcopy /E /I /Y build\\Warlock_artefacts\\Release\\VST3\\Warlock.vst3 ^
  "C:\\Program Files\\Common Files\\VST3\\Warlock.vst3"`}
          </pre>
          <p className="font-body mt-3 text-sm text-blood">
            Do not install into Program Files\Nihil Audio. Do not flatten the
            inner DLL to the VST3 root.
          </p>
        </Section>

        <Section n="6" title="Screenshots">
          <div className="grid gap-4 sm:grid-cols-2">
            {SHOTS.map((s) => (
              <figure key={s.src} className="border border-line bg-void">
                <img src={s.src} alt={s.cap} className="w-full" />
                <figcaption className="font-body px-2 py-1.5 text-xs text-bone-dim">
                  {s.cap}
                </figcaption>
              </figure>
            ))}
          </div>
        </Section>

        <Section n="7" title="Rescan">
          <Table
            rows={[
              ["Cubase", "Studio → VST Plug-in Manager → Rescan"],
              ["Reaper", "Preferences → Plug-ins → VST → Clear cache / re-scan"],
              ["Live", "Preferences → Plug-Ins → Rescan"],
              ["FL Studio", "Options → Manage plugins → Find plugins"],
              ["Studio One", "Options → Locations → Rescan"],
              ["Bitwig", "Settings → Locations → VST3"],
            ]}
          />
          <p className="font-body mt-3 text-sm text-bone-dim">
            Vendor Nihil Audio. Category Distortion. Name Warlock.
          </p>
        </Section>

        <Section n="8" title="Did all data pack?">
          <ul className="font-body list-disc space-y-1 pl-5 text-sm text-bone-dim">
            <li>Contents\x86_64-win\Warlock.vst3 is hundreds of KB</li>
            <li>Snapshots 1× and 2.0× PNGs exist</li>
            <li>Editor shows logo, knobs, stomp — not grey circles</li>
            <li>CAB IR darkens the tone (three WAVs are in the DLL)</li>
          </ul>
        </Section>
      </article>
    </main>
  );
}

function Section({
  n,
  title,
  children,
}: {
  n: string;
  title: string;
  children: ReactNode;
}) {
  return (
    <section className="mt-10 border-t border-line pt-6">
      <h2 className="font-display text-sm tracking-[0.22em] text-bone uppercase">
        <span className="text-blood">{n}</span> {title}
      </h2>
      <div className="mt-3">{children}</div>
    </section>
  );
}

function Table({ rows }: { rows: [string, string][] }) {
  return (
    <table className="mt-2 w-full text-left text-sm">
      <tbody>
        {rows.map(([a, b]) => (
          <tr key={a} className="border-b border-line/60">
            <td className="font-display py-1.5 pr-3 text-xs tracking-wide text-frost">
              {a}
            </td>
            <td className="font-body py-1.5 text-bone-dim">{b}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
