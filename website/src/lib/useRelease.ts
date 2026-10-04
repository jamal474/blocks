import { useEffect, useState } from 'react';
import { FALLBACK_RELEASE, REPO } from '@/data/site';

export type PlatformId = 'macos' | 'windows' | 'linux';

export interface PlatformDownload {
  id: PlatformId;
  num: string;
  label: string;
  note: string;
  url: string;
  size: string | null;
}

export interface ReleaseState {
  version: string;
  publishedAt: string;
  downloads: Record<PlatformId, PlatformDownload>;
}

interface GithubAsset {
  name: string;
  browser_download_url: string;
  size: number;
}

interface GithubRelease {
  tag_name: string;
  published_at: string;
  assets: GithubAsset[];
}

export const PLATFORM_ORDER: PlatformId[] = ['macos', 'windows', 'linux'];

const PLATFORMS: Record<PlatformId, { num: string; label: string; note: string; file: string; bytes: number }> = {
  macos: { num: '01', label: 'macOS', note: 'Ad-hoc signed app bundle, in a zip.', file: 'zip', bytes: 1909680 },
  windows: { num: '02', label: 'Windows', note: 'Zip archive with the game and its resources.', file: 'zip', bytes: 838088 },
  linux: { num: '03', label: 'Linux', note: 'Tarball (.tar.gz) of the game and its resources.', file: 'tar.gz', bytes: 1310733 },
};

function formatSize(bytes: number) {
  return `${(bytes / 1024 / 1024).toFixed(1)} MB`;
}

/** Links for the shipped release, so the page works before (or without) the API call. */
function fallbackDownloads(version: string): Record<PlatformId, PlatformDownload> {
  const base = `https://github.com/${REPO}/releases/download/v${version}`;
  return PLATFORM_ORDER.reduce((acc, id) => {
    const p = PLATFORMS[id];
    acc[id] = {
      id,
      num: p.num,
      label: p.label,
      note: p.note,
      url: `${base}/blocks-${id}-v${version}.${p.file}`,
      size: formatSize(p.bytes),
    };
    return acc;
  }, {} as Record<PlatformId, PlatformDownload>);
}

/**
 * Reads the newest GitHub release so version, date and per-platform asset
 * links stay current. Keeps the shipped release if the call fails.
 */
export function useRelease(): ReleaseState {
  const [state, setState] = useState<ReleaseState>({
    ...FALLBACK_RELEASE,
    downloads: fallbackDownloads(FALLBACK_RELEASE.version),
  });

  useEffect(() => {
    let cancelled = false;

    fetch(`https://api.github.com/repos/${REPO}/releases/latest`, {
      headers: { Accept: 'application/vnd.github+json' },
    })
      .then((res) => (res.ok ? (res.json() as Promise<GithubRelease>) : Promise.reject(res.status)))
      .then((release) => {
        if (cancelled) return;
        const version = release.tag_name.replace(/^v/, '');
        const downloads = fallbackDownloads(version);
        PLATFORM_ORDER.forEach((id) => {
          const match = release.assets.find((a) => a.name.toLowerCase().includes(id));
          if (match) {
            downloads[id] = { ...downloads[id], url: match.browser_download_url, size: formatSize(match.size) };
          } else {
            downloads[id] = { ...downloads[id], url: `https://github.com/${REPO}/releases/latest`, size: null };
          }
        });
        setState({ version, publishedAt: release.published_at, downloads });
      })
      .catch(() => {
        /* keep the shipped release */
      });

    return () => {
      cancelled = true;
    };
  }, []);

  return state;
}

/** Best guess at the visitor's platform, used for the primary button and the outlined card. */
export function detectPlatform(): PlatformId | null {
  if (typeof navigator === 'undefined') return null;
  const ua = navigator.userAgent.toLowerCase();
  if (ua.includes('android') || ua.includes('iphone') || ua.includes('ipad')) return null;
  if (ua.includes('windows')) return 'windows';
  if (ua.includes('mac')) return 'macos';
  if (ua.includes('linux') || ua.includes('x11')) return 'linux';
  return null;
}
