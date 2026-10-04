import { ArrowUpRight } from 'lucide-react';
import Shell from './Shell';
import SectionHead from './SectionHead';
import { cn, formatDate } from '@/lib/utils';
import { BUILD_URL, RELEASES_URL } from '@/data/site';
import { PLATFORM_ORDER, type PlatformId, type ReleaseState } from '@/lib/useRelease';

interface DownloadProps {
  release: ReleaseState;
  detected: PlatformId | null;
}

export default function Download({ release, detected }: DownloadProps) {
  return (
    <Shell as="section" id="download" className="pt-section">
      <SectionHead
        number="03"
        title="Download"
        aside={
          <p className="lbl text-ink-dim">
            Version {release.version} · {formatDate(release.publishedAt)}
          </p>
        }
      />

      <div className="mt-7 grid grid-cols-[repeat(auto-fit,minmax(240px,1fr))] gap-5">
        {PLATFORM_ORDER.map((id) => {
          const d = release.downloads[id];
          const yours = detected === id;
          return (
            <a
              key={id}
              href={d.url}
              className={cn(
                'card-rise flex flex-col border bg-paper-soft p-6 text-ink',
                yours ? 'border-accent' : 'border-line'
              )}
            >
              <span className="flex min-h-[22px] items-start justify-between">
                <span className="lbl text-ink-dim">{d.num}</span>
                {yours && <span className="lbl text-accent">Your system</span>}
              </span>
              <span className="fx-title-i mt-7 block text-[30px] leading-none">{d.label}</span>
              <span className="mb-7 mt-2 block flex-1 text-sm leading-normal text-ink-muted">{d.note}</span>
              <span className="flex items-center justify-between border-t border-line-soft pt-3.5">
                <span className="lbl">Download ↓</span>
                <span className="lbl text-ink-dim">{d.size ?? '—'}</span>
              </span>
            </a>
          );
        })}
      </div>

      <div className="mt-5 flex flex-wrap gap-x-7 gap-y-2">
        <a href={RELEASES_URL} target="_blank" rel="noreferrer" className="lnk lbl inline-flex items-center gap-1 py-3">
          All releases <ArrowUpRight size={12} strokeWidth={1.8} aria-hidden="true" />
        </a>
        <a href={BUILD_URL} target="_blank" rel="noreferrer" className="lnk lbl inline-flex items-center gap-1 py-3">
          Build from source <ArrowUpRight size={12} strokeWidth={1.8} aria-hidden="true" />
        </a>
      </div>
    </Shell>
  );
}
