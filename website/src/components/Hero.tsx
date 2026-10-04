import { ArrowDown } from 'lucide-react';
import Shell from './Shell';
import CraneFigure from './CraneFigure';
import { REPO_URL } from '@/data/site';
import { facts } from '@/data/content';
import type { PlatformId, ReleaseState } from '@/lib/useRelease';

interface HeroProps {
  release: ReleaseState;
  detected: PlatformId | null;
}

export default function Hero({ release, detected }: HeroProps) {
  const primary = detected ? release.downloads[detected] : null;

  return (
    <Shell as="section" id="top" className="pt-[clamp(40px,7vw,88px)]">
      <div className="flex flex-wrap items-start gap-x-[72px] gap-y-12">
        <div className="min-w-0 flex-[999_1_520px]">
          <p className="lbl mb-6 text-accent">Game · Physics · C++20</p>

          <h1 className="fx-display text-display-lg text-ink">
            Stack a tower <span className="fx-display-i text-accent">against its own physics.</span>
          </h1>

          <p className="fx-body mt-7 max-w-reading text-lg text-ink-soft">
            Drop blocks off a swinging crane and build as high as you dare. A clean landing calms
            the building; every miss makes it sway a little more, until it{' '}
            <span className="fx-mark">goes over</span>.
          </p>

          <div className="mt-9 flex flex-wrap gap-3">
            <a href={primary?.url ?? '#download'} className="btn lbl">
              {primary ? `Download for ${primary.label}` : 'Download'}
              <ArrowDown size={14} strokeWidth={2} aria-hidden="true" />
            </a>
            <a href={REPO_URL} target="_blank" rel="noreferrer" className="btn-line lbl">
              Source on GitHub
            </a>
          </div>

          <p className="lbl mt-5 tracking-[0.08em] text-ink-dim">
            Free · macOS, Windows &amp; Linux · v{release.version}
          </p>
        </div>

        <aside className="min-w-0 flex-[1_1_300px]">
          <p className="lbl mb-3.5 text-ink-dim">At a glance</p>
          <dl className="border-t border-line">
            {facts(release.version).map(([key, value]) => (
              <div key={key} className="grid grid-cols-[104px_1fr] gap-3 border-b border-line-soft py-[11px]">
                <dt className="lbl tracking-[0.08em] text-ink-dim">{key}</dt>
                <dd className="m-0 text-[15px]">{value}</dd>
              </div>
            ))}
          </dl>
        </aside>
      </div>

      <CraneFigure />
    </Shell>
  );
}
