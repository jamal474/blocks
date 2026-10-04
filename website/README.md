# City Bloxx landing page

The landing page for City Bloxx, served at **https://sabo.sh/blocks/**. It
describes the game, shows how it plays and how it works, and links each
platform's download straight to the newest GitHub release.

React 18 + TypeScript, Vite and Tailwind CSS, in the shared Shabbir Landing
design (accent `#1f6a5e`). The C++ game in the rest of this repository is not
built here.

## Local development

```shell
cd website
npm install
npm run dev
```

The dev server runs at http://localhost:5175/blocks/. A bare `/` will 404;
`/blocks` without the trailing slash is redirected by a small dev-only plugin in
`vite.config.ts`, matching what Netlify and the edge router do in production.

| Command | What it does |
| --- | --- |
| `npm run dev` | Vite dev server with hot reload |
| `npm run build` | Type-check, then build into `build/blocks/` |
| `npm run serve` | Serve the production build locally |
| `npm run typecheck` | Type-check only |

## Layout

| Path | Holds |
| --- | --- |
| `src/data/site.ts` | Repo links, fallback release, author, sibling projects |
| `src/data/content.ts` | Nav, facts, steps, controls, internals, stack chips |
| `src/lib/useRelease.ts` | Reads the latest GitHub release; falls back to v2.0.0 |
| `src/components/` | Shell, SiteHeader, Hero, CraneFigure, SectionHead, HowItPlays, UnderTheHood, Download, SiteFooter |

Copy lives in `src/data/`; components only lay it out.

## The `/blocks/` base path

`vite.config.ts` sets `base` to `/blocks/` and builds into `build/blocks/`, so
publishing `build/` serves `build/blocks/index.html` at `/blocks/` and on-disk
paths match public URLs. `BASE_PATH` overrides the prefix at build time; it must
start and end with a slash. Reference files in `public/` with the `asset()`
helper in `src/lib/utils.ts` rather than hard-coding `/blocks/`.

## Deployment (Netlify)

| Setting | Value |
| --- | --- |
| Base directory | `website` |
| Build command | `npm run build` |
| Publish directory | `build` (relative to the base) |

Redirect `/blocks` to `/blocks/` (301) and add an SPA fallback from `/blocks/*`
to `/blocks/index.html` (200), as Chitr's `netlify.toml` does for `/chitr/`.
