import { createFileRoute } from "@tanstack/react-router";
import { PedalEnclosure } from "@/components/pedal/enclosure";
import { StudioDock } from "@/components/studio/studio";

export const Route = createFileRoute("/")({ component: Home });

function Home() {
  return (
    <main className="page-bg relative min-h-dvh overflow-x-hidden">
      <div className="relative z-10 mx-auto flex max-w-6xl flex-col px-4 pt-5 pb-8 sm:px-6 lg:px-8">
        <header className="mb-5 text-center">
          <p className="font-display text-xs tracking-[0.42em] text-blood uppercase">
            Warlock Audio · VST3
          </p>
          <a
            href="/howto"
            className="font-body mt-2 inline-block text-xs tracking-wide text-frost underline-offset-4 hover:text-bone hover:underline"
          >
            Pack and install on PC
          </a>
        </header>

        <div className="flex flex-1 flex-col items-center gap-6 lg:flex-row lg:items-start lg:justify-center lg:gap-10">
          <PedalEnclosure />
          <StudioDock />
        </div>

        <footer className="mt-8 border-t border-line pt-4 text-center">
          <p className="font-body text-xs tracking-wide text-bone-dim">
            Warlock Audio · Warlock Distortion VST3 1.1.0 · x64. Drag knobs, stomp
            to engage, double-click a knob to reset.
          </p>
          <p className="font-body mx-auto mt-3 max-w-2xl text-[0.7rem] leading-relaxed tracking-wide text-bone-dim/80">
            © 2026 Warlock Audio. All rights reserved. Warlock Distortion is provided
            as-is for musical use. CabinetIR.wav is a synthetic 48 kHz impulse and is
            not a sampled commercial cabinet. Processing stays in this browser; no
            audio is uploaded. VST is a trademark of Steinberg Media Technologies GmbH.
            This product is not affiliated with, endorsed by, or sponsored by Steinberg.
            JUCE is licensed separately.
          </p>
        </footer>
      </div>
    </main>
  );
}
