float progress : register(c0);
float4 spectrum_[8] : register(c1);
static float spectrum[32] = (float[32])spectrum_;

bool rect(int2 p, int l, int r, int b, int t)
{
  return (l <= p.x && p.x <= r && b <= p.y && p.y <= t);
}

float4 ps_main(in float2 u : TEXCOORD) : COLOR
{
  int2 p = u * float2(640, 320) - 0.5;
  float3 c = 0.15;
  if (rect(p, 14, 625, 30, 308))
	c = 0.3;
  if (rect(p, 15, 624, 31, 307))
	c = 0.1;	
  
  if (rect(p, 14, 625, 15, 17))
  {
    float s = (p.x - 14) / 610.0;
    if (s < progress)
	  c = lerp(float3(0.2, 0.3, 0.4), float3(0.8, 0.4, 0.5), s);
    else c = 0.1;
    if (p.y == 15) c += 0.15;
  }

  float l = progress * 606 + 14;
  if (rect(p, l, l + 6, 10, 21)) c = 0;
  if (rect(p, l, l + 5, 11, 21)) c = 0.4;
  if (rect(p, l + 1, l + 5, 11, 20)) c = 0.3;

  p = int2(p.x - 16, p.y - 32);
  float2 fq = p / float2(19, 5);
  int2 q = floor(fq);
  int2 r = p - q * float2(19, 5);
  if (q.x >= 0 && q.x < 32 && q.y >= 0 && q.y < 55)
  {
	c = float3(0.16, 0.18, 0.19);
	
	float v = 0.5;
    if (q.y < spectrum[q.x])
    {
      float2 s = q / float2(31, 59);
      c = lerp(float3(0.4, 0.12, 0.1), float3(0.1, 0.4, 0.5), s.x);
      c = lerp(c, c + 0.6, s.y);
    }

    if (r.x > 17 || r.y > 3) c = 0.15;
    
	float m = abs(fq.y / 55 - 0.5) * 2;
	c += pow(m, 12) * float3(0.2, 0.23, 0.26);
  }
  return float4(c, 1);
}
