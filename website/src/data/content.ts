export const NAV = [
  { id: 'play', num: '01', label: 'How it plays' },
  { id: 'hood', num: '02', label: 'Under the hood' },
  { id: 'download', num: '03', label: 'Download' },
];

export const facts = (version: string): [string, string][] => [
  ['Version', version],
  ['Language', 'C++20'],
  ['Engine', 'SFML 2.6 · Box2D 2.4'],
  ['Build', 'CMake 3.28+ · Conan 2'],
  ['Platforms', 'macOS · Windows · Linux'],
];

export const steps = [
  {
    title: 'Time the drop',
    body: 'The block leaves the crane with the swing it already had. Release at the right moment and it falls straight.',
  },
  {
    title: 'Land it centred',
    body: 'Blocks snap level where they land. How far off centre they land decides how much the building leans.',
  },
  {
    title: 'Keep it standing',
    body: 'Taller towers sway further. When instability reaches 1.0, the building goes over.',
  },
];

export const controls: [string, string][] = [
  ['Space', 'Drop block'],
  ['R', 'Rebuild after a collapse'],
  ['Esc', 'Quit'],
];

export const internals = [
  {
    title: 'A crane with no physics bodies',
    body: 'The arm is a scripted pendulum, θ″ = −(g/L) sin θ, stepped with symplectic Euler. The swing neither decays nor winds up.',
  },
  {
    title: 'One flexing structure',
    body: 'Settled blocks follow a cantilever bending profile, so the upper floors move most and the tower bends as a whole rather than as loose pieces.',
  },
  {
    title: 'Box2D only for the fall',
    body: 'Full rigid-body physics takes over only when the building collapses. The camera follows the tower up with a smooth pan.',
  },
  {
    title: 'Released by CI',
    body: 'Conan 2 and CMake build it; GitHub Actions publishes macOS, Windows and Linux builds on every tag, with the macOS bundle ad-hoc signed.',
  },
];

export const stack = ['C++20', 'SFML', 'Box2D', 'CMake', 'Conan 2', 'GitHub Actions'];
