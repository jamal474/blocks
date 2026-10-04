export const REPO = 'jamal474/blocks';
export const REPO_URL = `https://github.com/${REPO}`;
export const RELEASES_URL = `${REPO_URL}/releases`;
export const LATEST_URL = `${RELEASES_URL}/latest`;
export const BUILD_URL = `${REPO_URL}#build-instructions`;

/** Shipped release, used until the live release lookup resolves. */
export const FALLBACK_RELEASE = {
  version: '2.0.0',
  publishedAt: '2026-09-12T16:03:18Z',
};

export const AUTHOR = {
  name: 'Md Shabbir Jamal',
  site: 'https://shabbirjamal.com',
};

/** Sibling landing pages on the short domain. */
export const MORE_PROJECTS = [
  { name: 'Chitr', note: 'Media player', url: 'https://sabo.sh/chitr' },
  { name: 'Chess', note: 'Online chess', url: 'https://sabo.sh/chess' },
  { name: 'All projects', note: 'Portfolio', url: 'https://shabbirjamal.com/projects' },
];
