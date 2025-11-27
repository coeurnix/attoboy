//==============================================================================
// example_directx.cpp - DirectX 11 Spinning Sphere with libattoboy Arguments
//==============================================================================
// Demonstrates that libattoboy can be used alongside standard libraries.
// Displays a spinning 3D sphere with random vertex colors using DirectX 11.
// Uses attoboy::Arguments for command-line parsing.
//
// Command-line options:
//   -w, --width     Viewport width (default: 800)
//   -h, --height    Viewport height (default: 600)
//   -s, --speed     Rotation speed multiplier (default: 1.0)
//   -f, --wireframe Enable wireframe rendering (default: false)
//
//==============================================================================

#include <attoboy/attoboy.h>

// Standard Windows and DirectX headers
#include <d3d11.h>
#include <d3dcompiler.h>
#include <math.h>
#include <windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace attoboy;

// Vertex structure with position and color
struct Vertex {
  float x, y, z;    // Position
  float r, g, b, a; // Color
};

// Constant buffer for transformation matrix
struct ConstantBuffer {
  float worldViewProj[16];
};

// Global DirectX objects
ID3D11Device *g_device = nullptr;
ID3D11DeviceContext *g_context = nullptr;
IDXGISwapChain *g_swapChain = nullptr;
ID3D11RenderTargetView *g_renderTarget = nullptr;
ID3D11DepthStencilView *g_depthStencil = nullptr;
ID3D11VertexShader *g_vertexShader = nullptr;
ID3D11PixelShader *g_pixelShader = nullptr;
ID3D11InputLayout *g_inputLayout = nullptr;
ID3D11Buffer *g_vertexBuffer = nullptr;
ID3D11Buffer *g_indexBuffer = nullptr;
ID3D11Buffer *g_constantBuffer = nullptr;
ID3D11RasterizerState *g_rasterizerState = nullptr;
ID3D11DepthStencilState *g_depthStencilState = nullptr;

int g_indexCount = 0;
float g_rotationSpeed = 1.0f;
bool g_wireframe = false;
bool g_running = true;

// Simple shader source code
const char *g_shaderSource = R"(
cbuffer ConstantBuffer : register(b0) {
    row_major float4x4 worldViewProj;  // row_major matches C++ memory layout
};

struct VS_INPUT {
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input) {
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    output.color = input.color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target {
    return input.color;
}
)";

// Matrix multiplication helper
void MatrixMultiply(float *result, const float *a, const float *b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i * 4 + j] = 0;
      for (int k = 0; k < 4; k++) {
        result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
      }
    }
  }
}

// Create identity matrix
void MatrixIdentity(float *m) {
  for (int i = 0; i < 16; i++)
    m[i] = 0;
  m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// Create rotation matrix around Y axis
void MatrixRotationY(float *m, float angle) {
  MatrixIdentity(m);
  float c = cosf(angle);
  float s = sinf(angle);
  m[0] = c;
  m[2] = s;
  m[8] = -s;
  m[10] = c;
}

// Create rotation matrix around X axis
void MatrixRotationX(float *m, float angle) {
  MatrixIdentity(m);
  float c = cosf(angle);
  float s = sinf(angle);
  m[5] = c;
  m[6] = -s;
  m[9] = s;
  m[10] = c;
}

// Create perspective projection matrix
void MatrixPerspective(float *m, float fov, float aspect, float nearZ,
                       float farZ) {
  for (int i = 0; i < 16; i++)
    m[i] = 0;
  float yScale = 1.0f / tanf(fov * 0.5f);
  float xScale = yScale / aspect;
  m[0] = xScale;
  m[5] = yScale;
  m[10] = farZ / (farZ - nearZ);
  m[11] = 1.0f;
  m[14] = (-nearZ * farZ) / (farZ - nearZ);
}

// Create translation matrix
void MatrixTranslation(float *m, float x, float y, float z) {
  MatrixIdentity(m);
  m[12] = x;
  m[13] = y;
  m[14] = z;
}

// Generate sphere mesh with random vertex colors
void CreateSphereMesh(Vertex **vertices, int *vertexCount,
                      unsigned int **indices, int *indexCount, int stacks,
                      int slices, float radius) {

  int numVerts = (stacks + 1) * (slices + 1);
  int numIndices = stacks * slices * 6;

  *vertices = new Vertex[numVerts];
  *indices = new unsigned int[numIndices];
  *vertexCount = numVerts;
  *indexCount = numIndices;

  // Generate vertices
  int v = 0;
  for (int stack = 0; stack <= stacks; stack++) {
    float phi = 3.14159f * stack / stacks;
    float y = radius * cosf(phi);
    float r = radius * sinf(phi);

    for (int slice = 0; slice <= slices; slice++) {
      float theta = 2.0f * 3.14159f * slice / slices;
      float x = r * cosf(theta);
      float z = r * sinf(theta);

      (*vertices)[v].x = x;
      (*vertices)[v].y = y;
      (*vertices)[v].z = z;

      // Random color for each vertex (using attoboy Math)
      (*vertices)[v].r = Math::RandomFloat();
      (*vertices)[v].g = Math::RandomFloat();
      (*vertices)[v].b = Math::RandomFloat();
      (*vertices)[v].a = 1.0f;

      v++;
    }
  }

  // Generate indices
  int idx = 0;
  for (int stack = 0; stack < stacks; stack++) {
    for (int slice = 0; slice < slices; slice++) {
      int first = stack * (slices + 1) + slice;
      int second = first + slices + 1;

      (*indices)[idx++] = first;
      (*indices)[idx++] = second;
      (*indices)[idx++] = first + 1;

      (*indices)[idx++] = second;
      (*indices)[idx++] = second + 1;
      (*indices)[idx++] = first + 1;
    }
  }
}

// Initialize DirectX
bool InitDirectX(HWND hwnd, int width, int height) {
  // Create swap chain description
  DXGI_SWAP_CHAIN_DESC scd = {};
  scd.BufferCount = 1;
  scd.BufferDesc.Width = width;
  scd.BufferDesc.Height = height;
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.BufferDesc.RefreshRate.Numerator = 60;
  scd.BufferDesc.RefreshRate.Denominator = 1;
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.OutputWindow = hwnd;
  scd.SampleDesc.Count = 4; // 4x MSAA
  scd.SampleDesc.Quality = 0;
  scd.Windowed = TRUE;

  // Create device and swap chain
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
      D3D11_SDK_VERSION, &scd, &g_swapChain, &g_device, &featureLevel,
      &g_context);

  if (FAILED(hr))
    return false;

  // Create render target view
  ID3D11Texture2D *backBuffer;
  g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
  g_device->CreateRenderTargetView(backBuffer, nullptr, &g_renderTarget);
  backBuffer->Release();

  // Create depth stencil texture
  D3D11_TEXTURE2D_DESC depthDesc = {};
  depthDesc.Width = width;
  depthDesc.Height = height;
  depthDesc.MipLevels = 1;
  depthDesc.ArraySize = 1;
  depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthDesc.SampleDesc.Count = 4;
  depthDesc.SampleDesc.Quality = 0;
  depthDesc.Usage = D3D11_USAGE_DEFAULT;
  depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

  ID3D11Texture2D *depthTexture;
  g_device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);
  g_device->CreateDepthStencilView(depthTexture, nullptr, &g_depthStencil);
  depthTexture->Release();

  // Set render targets
  g_context->OMSetRenderTargets(1, &g_renderTarget, g_depthStencil);

  // Set viewport
  D3D11_VIEWPORT viewport = {};
  viewport.Width = (float)width;
  viewport.Height = (float)height;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  g_context->RSSetViewports(1, &viewport);

  // Compile shaders
  ID3DBlob *vsBlob = nullptr;
  ID3DBlob *psBlob = nullptr;
  ID3DBlob *errorBlob = nullptr;

  hr = D3DCompile(g_shaderSource, strlen(g_shaderSource), nullptr, nullptr,
                  nullptr, "VSMain", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
  if (FAILED(hr)) {
    if (errorBlob)
      errorBlob->Release();
    return false;
  }

  hr = D3DCompile(g_shaderSource, strlen(g_shaderSource), nullptr, nullptr,
                  nullptr, "PSMain", "ps_4_0", 0, 0, &psBlob, &errorBlob);
  if (FAILED(hr)) {
    vsBlob->Release();
    if (errorBlob)
      errorBlob->Release();
    return false;
  }

  // Create shaders
  g_device->CreateVertexShader(vsBlob->GetBufferPointer(),
                               vsBlob->GetBufferSize(), nullptr,
                               &g_vertexShader);
  g_device->CreatePixelShader(psBlob->GetBufferPointer(),
                              psBlob->GetBufferSize(), nullptr, &g_pixelShader);

  // Create input layout
  D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0}};

  g_device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(),
                              vsBlob->GetBufferSize(), &g_inputLayout);

  vsBlob->Release();
  psBlob->Release();

  // Create sphere mesh
  Vertex *vertices;
  unsigned int *indices;
  int vertexCount;
  CreateSphereMesh(&vertices, &vertexCount, &indices, &g_indexCount, 20, 30,
                   1.0f);

  // Create vertex buffer
  D3D11_BUFFER_DESC vbDesc = {};
  vbDesc.Usage = D3D11_USAGE_DEFAULT;
  vbDesc.ByteWidth = sizeof(Vertex) * vertexCount;
  vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

  D3D11_SUBRESOURCE_DATA vbData = {};
  vbData.pSysMem = vertices;
  g_device->CreateBuffer(&vbDesc, &vbData, &g_vertexBuffer);

  // Create index buffer
  D3D11_BUFFER_DESC ibDesc = {};
  ibDesc.Usage = D3D11_USAGE_DEFAULT;
  ibDesc.ByteWidth = sizeof(unsigned int) * g_indexCount;
  ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

  D3D11_SUBRESOURCE_DATA ibData = {};
  ibData.pSysMem = indices;
  g_device->CreateBuffer(&ibDesc, &ibData, &g_indexBuffer);

  delete[] vertices;
  delete[] indices;

  // Create constant buffer
  D3D11_BUFFER_DESC cbDesc = {};
  cbDesc.Usage = D3D11_USAGE_DYNAMIC;
  cbDesc.ByteWidth = sizeof(ConstantBuffer);
  cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  g_device->CreateBuffer(&cbDesc, nullptr, &g_constantBuffer);

  // Create rasterizer state
  D3D11_RASTERIZER_DESC rsDesc = {};
  rsDesc.FillMode = g_wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
  rsDesc.CullMode = D3D11_CULL_BACK;
  rsDesc.FrontCounterClockwise = FALSE;
  rsDesc.MultisampleEnable = TRUE;
  g_device->CreateRasterizerState(&rsDesc, &g_rasterizerState);

  // Create depth stencil state
  D3D11_DEPTH_STENCIL_DESC dsDesc = {};
  dsDesc.DepthEnable = TRUE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
  g_device->CreateDepthStencilState(&dsDesc, &g_depthStencilState);

  return true;
}

// Render frame
void RenderFrame(float time, int width, int height) {
  // Clear background (dark blue)
  float clearColor[] = {0.05f, 0.05f, 0.15f, 1.0f};
  g_context->ClearRenderTargetView(g_renderTarget, clearColor);
  g_context->ClearDepthStencilView(g_depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

  // Build transformation matrices
  float rotY[16], rotX[16], world[16], view[16], proj[16];
  float worldView[16], worldViewProj[16];

  MatrixRotationY(rotY, time * g_rotationSpeed);
  MatrixRotationX(rotX, time * g_rotationSpeed * 0.3f);
  MatrixMultiply(world, rotX, rotY);

  MatrixTranslation(view, 0.0f, 0.0f, 3.0f);

  float aspect = (float)width / (float)height;
  MatrixPerspective(proj, 3.14159f / 4.0f, aspect, 0.1f, 100.0f);

  MatrixMultiply(worldView, world, view);
  MatrixMultiply(worldViewProj, worldView, proj);

  // Update constant buffer
  D3D11_MAPPED_SUBRESOURCE mapped;
  g_context->Map(g_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  memcpy(mapped.pData, worldViewProj, sizeof(float) * 16);
  g_context->Unmap(g_constantBuffer, 0);

  // Set pipeline state
  g_context->IASetInputLayout(g_inputLayout);
  g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  g_context->IASetVertexBuffers(0, 1, &g_vertexBuffer, &stride, &offset);
  g_context->IASetIndexBuffer(g_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  g_context->VSSetShader(g_vertexShader, nullptr, 0);
  g_context->VSSetConstantBuffers(0, 1, &g_constantBuffer);
  g_context->PSSetShader(g_pixelShader, nullptr, 0);

  g_context->RSSetState(g_rasterizerState);
  g_context->OMSetDepthStencilState(g_depthStencilState, 0);

  // Draw sphere
  g_context->DrawIndexed(g_indexCount, 0, 0);

  // Present
  g_swapChain->Present(1, 0);
}

// Cleanup DirectX
void CleanupDirectX() {
  if (g_depthStencilState)
    g_depthStencilState->Release();
  if (g_rasterizerState)
    g_rasterizerState->Release();
  if (g_constantBuffer)
    g_constantBuffer->Release();
  if (g_indexBuffer)
    g_indexBuffer->Release();
  if (g_vertexBuffer)
    g_vertexBuffer->Release();
  if (g_inputLayout)
    g_inputLayout->Release();
  if (g_pixelShader)
    g_pixelShader->Release();
  if (g_vertexShader)
    g_vertexShader->Release();
  if (g_depthStencil)
    g_depthStencil->Release();
  if (g_renderTarget)
    g_renderTarget->Release();
  if (g_swapChain)
    g_swapChain->Release();
  if (g_context)
    g_context->Release();
  if (g_device)
    g_device->Release();
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_DESTROY:
    g_running = false;
    PostQuitMessage(0);
    return 0;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
      g_running = false;
      PostQuitMessage(0);
    }
    return 0;
  }
  return DefWindowProcA(hwnd, msg, wParam, lParam);
}

// Standard main entry point (using standard libraries)
int main(int argc, char *argv[]) {
  // Parse command-line arguments using attoboy::Arguments
  Arguments args;
  args.addParameter("w", "Viewport width in pixels", "800", "width");
  args.addParameter("h", "Viewport height in pixels", "600", "height");
  args.addParameter("s", "Rotation speed multiplier", "1.0", "speed");
  args.addFlag("f", "Enable wireframe rendering", false, "wireframe");
  args.setHelp("DirectX 11 Spinning Sphere Demo\n"
               "Displays a colorful spinning 3D sphere.\n"
               "Press ESC to exit.");

  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    return 0; // Help was shown or error
  }

  int width = parsed.get<String, String>("w", "800").toInteger();
  int height = parsed.get<String, String>("h", "600").toInteger();
  g_rotationSpeed = parsed.get<String, String>("s", "1.0").toFloat();
  g_wireframe = parsed.get<String, String>("f", "false").toBool();

  // Clamp dimensions to reasonable values
  width = Math::Clamp(width, 320, 3840);
  height = Math::Clamp(height, 240, 2160);

  // Register window class
  WNDCLASSEXA wc = {};
  wc.cbSize = sizeof(WNDCLASSEXA);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandleA(nullptr);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.lpszClassName = "DX11SphereClass";
  RegisterClassExA(&wc);

  // Calculate window size for desired client area
  RECT rect = {0, 0, width, height};
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

  // Create window
  HWND hwnd = CreateWindowExA(
      0, "DX11SphereClass", "DirectX 11 - Spinning Sphere", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
      rect.bottom - rect.top, nullptr, nullptr, wc.hInstance, nullptr);

  if (!hwnd) {
    MessageBoxA(nullptr, "Failed to create window", "Error", MB_OK);
    return 1;
  }

  // Initialize DirectX
  if (!InitDirectX(hwnd, width, height)) {
    MessageBoxA(nullptr, "Failed to initialize DirectX 11", "Error", MB_OK);
    return 1;
  }

  // Show window
  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  // Main loop
  LARGE_INTEGER frequency, startTime, currentTime;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&startTime);

  MSG msg = {};
  while (g_running) {
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }

    // Calculate elapsed time
    QueryPerformanceCounter(&currentTime);
    float time = (float)(currentTime.QuadPart - startTime.QuadPart) /
                 (float)frequency.QuadPart;

    RenderFrame(time, width, height);
  }

  // Cleanup
  CleanupDirectX();
  DestroyWindow(hwnd);
  UnregisterClassA("DX11SphereClass", wc.hInstance);

  return 0;
}