// Bandeiras em SVG. Emojis de bandeira (🇧🇷, 🇺🇸...) não existem no Windows,
// que mostra apenas as letras do país — por isso desenhamos as bandeiras aqui.

function starPoints(cx, cy, R, r, rot = -90) {
  const pts = [];
  for (let i = 0; i < 10; i++) {
    const rad = ((rot + i * 36) * Math.PI) / 180;
    const radius = i % 2 === 0 ? R : r;
    pts.push(`${(cx + radius * Math.cos(rad)).toFixed(2)},${(cy + radius * Math.sin(rad)).toFixed(2)}`);
  }
  return pts.join(" ");
}

const FLAGS = {
  us: (
    <>
      <rect width="24" height="16" fill="#fff" />
      {[0, 2, 4, 6, 8, 10, 12].map((i) => (
        <rect key={i} y={(i * 16) / 13} width="24" height={16 / 13} fill="#B22234" />
      ))}
      <rect width="11" height="8.6" fill="#3C3B6E" />
      {[0, 1, 2, 3].map((r) =>
        [0, 1, 2, 3, 4].map((c) => (
          <circle key={`${r}-${c}`} cx={1.6 + c * 2.1 + (r % 2) * 1} cy={1.5 + r * 1.9} r="0.4" fill="#fff" />
        ))
      )}
    </>
  ),
  br: (
    <>
      <rect width="24" height="16" fill="#009C3B" />
      <polygon points="12,1.8 22,8 12,14.2 2,8" fill="#FFDF00" />
      <circle cx="12" cy="8" r="3.7" fill="#002776" />
      <path d="M8.5 7.2 Q12 6 15.5 9" stroke="#fff" strokeWidth="0.8" fill="none" />
    </>
  ),
  es: (
    <>
      <rect width="24" height="16" fill="#AA151B" />
      <rect y="4" width="24" height="8" fill="#F1BF00" />
    </>
  ),
  fr: (
    <>
      <rect width="8" height="16" fill="#0055A4" />
      <rect x="8" width="8" height="16" fill="#fff" />
      <rect x="16" width="8" height="16" fill="#EF4135" />
    </>
  ),
  jp: (
    <>
      <rect width="24" height="16" fill="#fff" />
      <circle cx="12" cy="8" r="4.6" fill="#BC002D" />
    </>
  ),
  cn: (
    <>
      <rect width="24" height="16" fill="#DE2910" />
      <polygon points={starPoints(4.6, 4.4, 2.6, 1.05)} fill="#FFDE00" />
      <polygon points={starPoints(9.4, 1.9, 0.9, 0.36, -70)} fill="#FFDE00" />
      <polygon points={starPoints(11.2, 3.9, 0.9, 0.36, -45)} fill="#FFDE00" />
      <polygon points={starPoints(11.2, 6.4, 0.9, 0.36, -20)} fill="#FFDE00" />
      <polygon points={starPoints(9.4, 8.3, 0.9, 0.36, -85)} fill="#FFDE00" />
    </>
  ),
};

export default function Flag({ code, height = 14 }) {
  const art = FLAGS[code];
  if (!art) return null;

  return (
    <svg
      width={(height * 24) / 16}
      height={height}
      viewBox="0 0 24 16"
      aria-hidden="true"
      style={{
        flexShrink: 0,
        borderRadius: 3,
        boxShadow: "0 0 0 1px rgba(0,0,0,0.18)",
        display: "block",
      }}
    >
      {art}
    </svg>
  );
}
