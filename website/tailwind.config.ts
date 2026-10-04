import type { Config } from 'tailwindcss';

// Shabbir Landing tokens. Only `accent` differs between project sites.
const config: Config = {
  content: ['./index.html', './src/**/*.{ts,tsx}'],
  theme: {
    extend: {
      colors: {
        paper: { DEFAULT: '#f0ede4', soft: '#f7f5ef', deep: '#e6e2d5' },
        ink: { DEFAULT: '#14141a', soft: '#2b2b31', muted: '#5c594e', dim: '#6a675d' },
        line: { DEFAULT: '#cec9b6', soft: '#e2ddca' },
        signal: '#f7d600',
        accent: '#1f6a5e', // City Bloxx
      },
      fontFamily: {
        serif: ['"Fraunces"', 'Georgia', 'ui-serif', 'serif'],
        mono: ['"JetBrains Mono"', 'ui-monospace', 'SFMono-Regular', 'monospace'],
      },
      fontSize: {
        'display-lg': ['clamp(44px, 6.4vw, 84px)', { lineHeight: '0.95', letterSpacing: '-0.02em' }],
        'display-md': ['clamp(32px, 4.4vw, 56px)', { lineHeight: '1', letterSpacing: '-0.02em' }],
      },
      maxWidth: { shell: '1180px', reading: '58ch' },
      padding: {
        gutter: 'clamp(20px, 4vw, 40px)',
        section: 'clamp(80px, 10vw, 128px)',
      },
      boxShadow: { lift: '0 14px 32px rgba(20, 20, 26, 0.10)' },
      keyframes: {
        swing: { from: { transform: 'rotate(-16deg)' }, to: { transform: 'rotate(16deg)' } },
        sway: { from: { transform: 'rotate(-1.2deg)' }, to: { transform: 'rotate(1.2deg)' } },
      },
      animation: {
        swing: 'swing 2.6s ease-in-out infinite alternate',
        sway: 'sway 5.2s ease-in-out infinite alternate',
      },
    },
  },
  plugins: [],
};

export default config;
