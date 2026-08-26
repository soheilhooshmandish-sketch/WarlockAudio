import { useEffect, useRef } from "react";
import { getEngine } from "@/lib/audio/pedal-engine";
import { cn } from "@/lib/utils";

const SEGMENTS = 8;

export function LedMeter({ engaged }: { engaged: boolean }) {
  const refs = useRef<(HTMLSpanElement | null)[]>([]);

  useEffect(() => {
    const wave = new Uint8Array(256);
    let timer = 0;
    const draw = () => {
      const engine = getEngine();
      let peak = 0;
      if (engine && engaged) {
        engine.analyser.getByteTimeDomainData(wave);
        for (let i = 0; i < wave.length; i += 4) {
          peak = Math.max(peak, Math.abs(wave[i] - 128) / 128);
        }
      }
      const lit = Math.round(peak * SEGMENTS);
      refs.current.forEach((el, i) => {
        if (!el) return;
        el.classList.toggle("on", i < lit);
      });
    };
    timer = window.setInterval(draw, 80);
    draw();
    return () => window.clearInterval(timer);
  }, [engaged]);

  return (
    <div className="led-meter" aria-hidden>
      {Array.from({ length: SEGMENTS }).map((_, i) => {
        const idx = SEGMENTS - 1 - i;
        const tone = idx >= 6 ? "red" : idx >= 4 ? "amber" : "green";
        return (
          <span
            key={idx}
            ref={(el) => {
              refs.current[idx] = el;
            }}
            className={cn("led-seg", tone)}
          />
        );
      })}
    </div>
  );
}
