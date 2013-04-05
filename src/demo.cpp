#include "common.hpp"
#include "demo.hpp"
#include "mzk.hpp"

#ifndef COMPILE_SHADER_FROM_FILE
#include "shader.hpp"
#endif

static const float quad_vertices[] = {
	 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static IDirect3DVertexShader9* g_vertex_shader = NULL;
static IDirect3DPixelShader9* g_pixel_shader = NULL;

extern D3DPRESENT_PARAMETERS g_dev_params;

void DemoInit(IDirect3DDevice9* device)
{
	ID3DXBuffer *vs_buffer = NULL;
	ID3DXBuffer *ps_buffer = NULL;

#ifdef COMPILE_SHADER_FROM_FILE
	D3DXCompileShaderFromFile("fx/vs.hlsl", NULL, NULL, "vs_main", "vs_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, &vs_buffer, NULL, NULL);
	D3DXCompileShaderFromFile("fx/ps.hlsl", NULL, NULL, "ps_main", "ps_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, &ps_buffer, NULL, NULL);

#else
	D3DXCompileShader(vs_codes, sizeof(ps_codes), NULL, NULL, "vs_main", "vs_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, &vs_buffer, NULL, NULL);
	D3DXCompileShader(ps_codes, sizeof(ps_codes), NULL, NULL, "ps_main", "ps_3_0", D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, &ps_buffer, NULL, NULL);

#endif  // COMPILE_SHADER_FROM_FILE

	device->CreateVertexShader((DWORD*)vs_buffer->GetBufferPointer(), &g_vertex_shader);
	device->CreatePixelShader((DWORD*)ps_buffer->GetBufferPointer(), &g_pixel_shader);

	InitSound();
}

static float spectrum[32];
float spectrum_tmp[STFT_WINDOW_SIZE];

void DemoUpdate(IDirect3DDevice9* device, long time)
{
	device->BeginScene();
	device->SetVertexShader(g_vertex_shader);
	device->SetPixelShader(g_pixel_shader);

	float progress[4];
	progress[0] = GetCurrentSample() / float(MAX_SAMPLES);
	device->SetPixelShaderConstantF(0, progress, 1);

	GetSpectrum(spectrum_tmp);
	int bin_size = STFT_WINDOW_SIZE / 32;
	for (int i = 0; i != 32; ++i)
	{
		float value = 0;
		for (int j = 0; j < bin_size; ++j)
		{
			int k = i * bin_size + j;
			value += spectrum_tmp[k];
		}
		value /= bin_size;
		value *= 8.0f;
		spectrum[i] = spectrum[i] * 0.75f + value * 0.25f;
	}

	device->SetPixelShaderConstantF(1, spectrum, 8);

	device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad_vertices, sizeof(float) * 5);
	device->EndScene();
	device->Present(NULL, NULL, NULL, NULL);
}

void DemoDestroy(IDirect3DDevice9* device)
{
	g_vertex_shader->Release();
	g_pixel_shader->Release();
}
