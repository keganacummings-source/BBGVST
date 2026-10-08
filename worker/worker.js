/**
 * BabyGirl - DreamAPI Cloudflare Worker
 * Cloud preset sharing, community catalog, and Discord webhook integration
 * Based on keganacummings-source/Hush DreamAPI backend
 */

export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);
    const corsHeaders = {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type, Authorization',
    };

    if (request.method === 'OPTIONS') {
      return new Response(null, { headers: corsHeaders });
    }

    // Health check
    if (url.pathname === '/' || url.pathname === '/health') {
      return new Response(JSON.stringify({ status: 'online', service: 'BabyGirl DreamAPI', version: '2.4.0' }), {
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      });
    }

    // List & search presets: GET /api/presets?query=&tag=
    if (url.pathname === '/api/presets' && request.method === 'GET') {
      const query = url.searchParams.get('query')?.toLowerCase() || '';
      const tag = url.searchParams.get('tag')?.toLowerCase() || '';

      const presets = [
        {
          id: 'bg-sig-01',
          name: 'BabyGirl Signature',
          author: 'Kegana Cummings',
          category: 'Producer Lead',
          tags: ['tube', 'tape', 'master'],
          stars: 184,
          date: '2026-10-07',
          patch: {
            tubeDrive: 4.2,
            filterCutoff: 1850,
            filterReso: 0.48,
            tapeTime: 0.36,
            tapeFlutter: 0.32,
            vinylCrackle: 0.15,
            tapeWarp: 0.22,
            shimmerMix: 0.35,
            stereoWidth: 1.25,
            ironDrive: 0.45
          }
        },
        {
          id: 'kyoto-02',
          name: 'Kyoto Shrine Spirit',
          author: 'KyotoSpxrit',
          category: 'Lofi Ambient',
          tags: ['lofi', 'shimmer', 'vinyl', 'nostalgic'],
          stars: 142,
          date: '2026-10-06',
          patch: {
            tubeDrive: 2.2,
            filterCutoff: 1250,
            filterReso: 0.32,
            tapeTime: 0.54,
            tapeFlutter: 0.65,
            vinylCrackle: 0.48,
            tapeWarp: 0.55,
            shimmerMix: 0.72,
            stereoWidth: 1.4,
            ironDrive: 0.25
          }
        },
        {
          id: 'hush-03',
          name: 'Hush Overdrive Furnace',
          author: 'Hush Core',
          category: 'Bass / Acid',
          tags: ['acid', 'overdrive', 'pentode', 'resonance'],
          stars: 129,
          date: '2026-10-05',
          patch: {
            tubeDrive: 8.2,
            filterCutoff: 950,
            filterReso: 0.84,
            tapeTime: 0.16,
            tapeFlutter: 0.10,
            vinylCrackle: 0.05,
            tapeWarp: 0.08,
            shimmerMix: 0.15,
            stereoWidth: 1.05,
            ironDrive: 0.70
          }
        },
        {
          id: 'master-04',
          name: 'Final Polish Master Suite',
          author: 'Master Labs',
          category: 'Mastering',
          tags: ['multiband', 'truepeak', 'iron', 'wide'],
          stars: 198,
          date: '2026-10-04',
          patch: {
            tubeDrive: 1.8,
            filterCutoff: 14000,
            filterReso: 0.15,
            tapeTime: 0.10,
            tapeFlutter: 0.08,
            vinylCrackle: 0.0,
            tapeWarp: 0.0,
            shimmerMix: 0.12,
            stereoWidth: 1.35,
            ironDrive: 0.55
          }
        }
      ];

      const filtered = presets.filter(p => {
        const matchesQ = !query || p.name.toLowerCase().includes(query) || p.author.toLowerCase().includes(query);
        const matchesTag = !tag || p.tags.some(t => t.toLowerCase().includes(tag));
        return matchesQ && matchesTag;
      });

      return new Response(JSON.stringify({ presets: filtered }), {
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      });
    }

    // Publish new preset: POST /api/presets
    if (url.pathname === '/api/presets' && request.method === 'POST') {
      try {
        const body = await request.json();
        const newPreset = {
          id: 'preset-' + Date.now(),
          name: body.name || 'Untitled Dream',
          author: body.author || 'Anonymous Producer',
          category: body.category || 'Custom',
          tags: body.tags || ['custom'],
          stars: 1,
          date: new Date().toISOString().split('T')[0],
          patch: body.patch || {}
        };

        // Notify Discord Webhook if configured
        if (env?.DISCORD_WEBHOOK_URL) {
          try {
            await fetch(env.DISCORD_WEBHOOK_URL, {
              method: 'POST',
              headers: { 'Content-Type': 'application/json' },
              body: JSON.stringify({
                embeds: [{
                  title: `🎛️ New BabyGirl Preset: ${newPreset.name}`,
                  description: `Published by **${newPreset.author}** in category **${newPreset.category}**`,
                  color: 0xf59e0b,
                  fields: [
                    { name: 'Tags', value: newPreset.tags.join(', ') || 'none', inline: true },
                    { name: 'Drive', value: `${newPreset.patch.tubeDrive || 3.8}x`, inline: true },
                    { name: 'Cutoff', value: `${newPreset.patch.filterCutoff || 1850} Hz`, inline: true },
                  ]
                }]
              })
            });
          } catch (e) {
            console.error('Discord webhook dispatch error:', e);
          }
        }

        return new Response(JSON.stringify({ success: true, preset: newPreset }), {
          headers: { ...corsHeaders, 'Content-Type': 'application/json' },
        });
      } catch (err) {
        return new Response(JSON.stringify({ error: err.message }), {
          status: 400,
          headers: { ...corsHeaders, 'Content-Type': 'application/json' },
        });
      }
    }

    return new Response('Not Found', { status: 404, headers: corsHeaders });
  }
};
