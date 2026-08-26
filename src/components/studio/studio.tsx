import { Download, Mic, Square, Upload } from "lucide-react";
import { useEffect, useRef, useState } from "react";
import { PRESETS } from "@/lib/audio/presets";
import { RIFFS } from "@/lib/audio/riff-synth";
import { cn } from "@/lib/utils";
import { Button } from "@/components/ui/button";
import { usePedal } from "@/store/pedal-store";
import { Spectrum } from "./visualizer";

export function StudioDock() {
  const fileRef = useRef<HTMLInputElement>(null);
  const [fileReady, setFileReady] = useState(false);
  const awake = usePedal((s) => s.awake);
  const engaged = usePedal((s) => s.engaged);
  const source = usePedal((s) => s.source);
  const activeRiff = usePedal((s) => s.activeRiff);
  const presetId = usePedal((s) => s.presetId);
  const micError = usePedal((s) => s.micError);
  const fileName = usePedal((s) => s.fileName);
  const playRiff = usePedal((s) => s.playRiff);
  const stopRiff = usePedal((s) => s.stopRiff);
  const startMic = usePedal((s) => s.startMic);
  const stopMic = usePedal((s) => s.stopMic);
  const loadFile = usePedal((s) => s.loadFile);
  const stopFile = usePedal((s) => s.stopFile);
  const loadPreset = usePedal((s) => s.loadPreset);
  const loadUserPreset = usePedal((s) => s.loadUserPreset);
  const saveUserPreset = usePedal((s) => s.saveUserPreset);
  const deleteUserPreset = usePedal((s) => s.deleteUserPreset);
  const userPresets = usePedal((s) => s.userPresets);
  const wake = usePedal((s) => s.wake);

  const live = awake && engaged;
  const [presetName, setPresetName] = useState("");

  useEffect(() => {
    setFileReady(true);
  }, []);

  return (
    <aside className="studio-panel flex w-full max-w-xl flex-col gap-5 lg:max-w-sm">
      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          Play a riff
        </h2>
        <p className="font-body mt-1 text-sm text-bone-dim">
          Stomp to wake it. Send a wall of ice through the chainsaw.
        </p>
        <div className="mt-3 grid grid-cols-2 gap-2">
          {RIFFS.map((riff) => {
            const on = activeRiff === riff.id;
            return (
              <button
                key={riff.id}
                type="button"
                onClick={() => {
                  if (on) stopRiff();
                  else void playRiff(riff.id);
                }}
                className={cn(
                  "flex min-h-12 flex-col items-start justify-center border px-3 py-2 text-left transition-colors duration-(--motion-quick)",
                  on
                    ? "border-blood bg-blood/15 text-bone"
                    : "border-line text-bone hover:border-blood/50",
                )}
              >
                <span className="font-display text-xs tracking-[0.16em] uppercase">
                  {riff.name}
                </span>
                <span className="font-body text-xs text-bone-dim italic">
                  {on ? "Playing — tap to stop" : riff.epithet}
                </span>
              </button>
            );
          })}
        </div>
        {!awake ? (
          <Button className="mt-3 w-full" variant="primary" onClick={() => void wake()}>
            Power the pedal
          </Button>
        ) : null}
      </section>

      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          Voicings
        </h2>
        <div className="mt-3 grid grid-cols-2 gap-1.5">
          {PRESETS.map((preset) => {
            const on = presetId === preset.id;
            return (
              <button
                key={preset.id}
                type="button"
                onClick={() => loadPreset(preset.id)}
                className={cn(
                  "min-h-11 border px-2.5 py-2 text-left transition-colors duration-(--motion-quick)",
                  on
                    ? "border-blood bg-blood text-bone"
                    : "border-line text-bone hover:border-frost/40",
                )}
              >
                <span className="font-display block text-xs tracking-[0.12em] uppercase">
                  {preset.name}
                </span>
                <span
                  className={cn(
                    "font-body block text-xs italic",
                    on ? "text-bone/70" : "text-bone-dim",
                  )}
                >
                  {preset.epithet}
                </span>
              </button>
            );
          })}
        </div>
      </section>

      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          User banks
        </h2>
        <form
          className="mt-3 flex gap-2"
          onSubmit={(e) => {
            e.preventDefault();
            saveUserPreset(presetName || "Unnamed");
            setPresetName("");
          }}
        >
          <input
            value={presetName}
            onChange={(e) => setPresetName(e.target.value)}
            maxLength={32}
            placeholder="Name this voicing"
            className="min-w-0 flex-1 border border-line bg-void/50 px-2 py-1.5 font-display text-xs tracking-wide text-bone placeholder:text-bone-dim"
          />
          <Button type="submit" size="sm" variant="primary">
            Save
          </Button>
        </form>
        {userPresets.length ? (
          <div className="mt-2 grid grid-cols-1 gap-1.5">
            {userPresets.map((preset) => {
              const on = presetId === preset.id;
              return (
                <div key={preset.id} className="flex gap-1.5">
                  <button
                    type="button"
                    onClick={() => loadUserPreset(preset.id)}
                    className={cn(
                      "min-h-10 flex-1 border px-2.5 py-1.5 text-left",
                      on ? "border-blood bg-blood/20 text-bone" : "border-line text-bone",
                    )}
                  >
                    <span className="font-display block text-xs tracking-[0.12em] uppercase">
                      {preset.name}
                    </span>
                  </button>
                  <button
                    type="button"
                    aria-label={`Delete ${preset.name}`}
                    onClick={() => deleteUserPreset(preset.id)}
                    className="border border-line px-2 font-display text-[0.6rem] tracking-wide text-bone-dim uppercase hover:text-blood"
                  >
                    X
                  </button>
                </div>
              );
            })}
          </div>
        ) : (
          <p className="font-body mt-2 text-xs text-bone-dim italic">
            Save a bank after you carve a tone.
          </p>
        )}
      </section>

      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          External signal
        </h2>
        <div className="mt-3 grid grid-cols-2 gap-2">
          <Button
            variant={source === "mic" ? "primary" : "ghost"}
            size="sm"
            onClick={() => {
              if (source === "mic") stopMic();
              else void startMic();
            }}
          >
            <Mic className="size-3.5" />
            {source === "mic" ? "Mute mic" : "Microphone"}
          </Button>
          <Button
            variant={source === "file" ? "primary" : "ghost"}
            size="sm"
            onClick={() => {
              if (source === "file") stopFile();
              else fileRef.current?.click();
            }}
          >
            {source === "file" ? (
              <Square className="size-3.5" />
            ) : (
              <Upload className="size-3.5" />
            )}
            {source === "file" ? "Eject" : "Load file"}
          </Button>
        </div>
        {fileReady ? (
          <input
            ref={fileRef}
            type="file"
            accept="audio/*,.wav,.mp3,.ogg,.flac,.m4a"
            className="sr-only"
            onChange={(e) => {
              const file = e.target.files?.[0];
              if (file) void loadFile(file);
              e.target.value = "";
            }}
          />
        ) : null}
        {fileName ? (
          <p className="font-body mt-2 truncate text-xs text-frost">{fileName}</p>
        ) : null}
        {micError ? (
          <p className="font-body mt-2 text-xs text-blood">{micError}</p>
        ) : null}
        {source === "mic" ? (
          <p className="font-body mt-2 text-xs text-bone-dim">
            Use headphones to avoid feedback.
          </p>
        ) : null}
      </section>

      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          Install on PC
        </h2>
        <p className="font-body mt-1 text-sm text-bone-dim">
          VST3 for 64-bit Windows. Chassis, knobs, stomp, logo, and three cab
          IRs compile into the plugin so the DAW cannot lose the skins.
        </p>
        <a
          href="/Warlock-VST3-PC-Export.zip"
          download
          className="mt-3 inline-flex min-h-11 w-full items-center justify-center gap-2 border border-blood bg-blood/20 px-3 font-display text-xs tracking-[0.16em] text-bone uppercase hover:bg-blood/35"
        >
          <Download className="size-3.5" />
          Download VST3 build kit
        </a>
        <ol className="font-body mt-3 list-decimal space-y-1 pl-4 text-xs text-bone-dim">
          <li>Unzip. Install Visual Studio 2022 C++ and CMake.</li>
          <li>Run <span className="text-bone">build.bat</span>.</li>
          <li>
            Copy the whole{" "}
            <span className="text-bone">Warlock.vst3</span> folder to{" "}
            <span className="text-bone">
              C:\Program Files\Common Files\VST3\
            </span>
          </li>
          <li>Rescan plugins. Category Distortion · Nihil Audio.</li>
        </ol>
      </section>

      <section>
        <h2 className="font-display text-xs tracking-[0.28em] text-frost uppercase">
          Output
        </h2>
        <div className="mt-2 border border-line bg-void/50 px-2 py-1">
          <Spectrum engaged={live} />
        </div>
      </section>
    </aside>
  );
}
