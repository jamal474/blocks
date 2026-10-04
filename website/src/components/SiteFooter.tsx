import Shell from './Shell';
import { AUTHOR, MORE_PROJECTS } from '@/data/site';

export default function SiteFooter() {
  return (
    <Shell as="footer" className="pb-12 pt-[clamp(96px,12vw,144px)]">
      <div className="flex flex-wrap justify-between gap-x-12 gap-y-8 border-t-2 border-ink pt-7">
        <div className="flex-[1_1_280px]">
          <p className="lbl mb-2.5 text-ink-dim">Made by</p>
          <a href={AUTHOR.site} className="fx-title-i text-[28px] leading-none transition-colors hover:text-accent">
            {AUTHOR.name} ↗
          </a>
        </div>

        <div className="flex-[1_1_320px]">
          <p className="lbl mb-1.5 text-ink-dim">More projects</p>
          {MORE_PROJECTS.map((p) => (
            <a
              key={p.name}
              href={p.url}
              className="row-nudge flex items-baseline justify-between border-b border-line-soft py-3"
            >
              <span className="text-xl italic">{p.name}</span>
              <span className="lbl text-ink-dim">{p.note}</span>
            </a>
          ))}
        </div>
      </div>
    </Shell>
  );
}
