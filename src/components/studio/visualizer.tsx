import { useEffect, useRef } from "react";
import { getEngine } from "@/lib/audio/pedal-engine";

const BAR_COUNT = 24;

export function Spectrum({ engaged }: { engaged: boolean }) {
  const barsRef = useRef<SVGGElement>(null);

  useEffect(() => {
    const group = barsRef.current;
    if (!group) return;
    const rects = Array.from(group.querySelectorAll("rect"));
    const freq = new Uint8Array(128);
    let timer = 0;

    const draw = () => {
      const engine = getEngine();
      if (engine && engaged) {
        engine.analyser.getByteFrequencyData(freq);
        for (let i = 0; i < rects.length; i++) {
          const v = freq[Math.floor(i * 3.2)] / 255;
          const bh = Math.max(2, v * 56);
          rects[i].setAttribute("y", String(56 - bh));
          rects[i].setAttribute("height", String(bh));
        }
      } else {
        for (const r of rects) {
          r.setAttribute("y", "54");
          r.setAttribute("height", "2");
        }
      }
    };
    timer = window.setInterval(draw, 80);
    draw();
    return () => window.clearInterval(timer);
  }, [engaged]);

  const gap = 3;
  const bw = (280 - gap * (BAR_COUNT - 1)) / BAR_COUNT;

  return (
    <svg viewBox="0 0 280 56" className="h-12 w-full" aria-hidden>
      <g ref={barsRef} fill="rgb(225, 29, 46)">
        {Array.from({ length: BAR_COUNT }).map((_, i) => (
          <rect key={i} x={i * (bw + gap)} y={54} width={bw} height={2} />
        ))}
      </g>
    </svg>
  );
}
