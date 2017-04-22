const char* SHADER_SRC_2D_VS = ""
    "input float2 inPosition;\n"
    "input float2 inTexCoord;\n"
    "input float4 inColor;\n"
    "\n"
    "output float2 outTexCoord;\n"
    "output float4 outColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    oPosition = mul(float4(inPosition.xy, 0.0, 1.0), oViewProjection);\n"
    "    outTexCoord = inTexCoord;\n"
    "    outColor = inColor;\n"
    "}\n"
"";

const char* SHADER_SRC_2D_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "input float2 inTexCoord;\n"
    "input float4 inColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 diffuse = texDiffuse(inTexCoord);\n"
    "    oColor = diffuse * inColor;\n"
    "}\n"
"";

const char* SHADER_SRC_EFFECTS_VS = ""
    "input float2 inPosition;\n"
    "\n"
    "output float2 outTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    oPosition = float4(inPosition.xy, 0.0, 1.0);\n"
    "    outTexCoord.x = (inPosition.x + 1.0) / 2.0;\n"
    "    outTexCoord.y = 1.0 - (inPosition.y + 1.0) / 2.0;\n"
    "}\n"
"";

const char* SHADER_SRC_SEPIA_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "extern float3 tone;\n"
    "extern float desaturate;\n"
    "extern float sepiaAmount;\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 colorInput = texDiffuse(inTexCoord);\n"
    "    float3 sepia = colorInput.rgb;\n"
    "\n"
    "    // calculating amounts of input, grey and sepia colors to blend and combine\n"
    "    float grey = dot(sepia, float3(0.2126, 0.7152, 0.0722));\n"
    "\n"
    "    float3 grayscale = (grey * desaturate) + (colorInput.rgb * (1.0 - desaturate));\n"
    "\n"
    "    sepia = lerp(sepia, grayscale, sepiaAmount);\n"
    "    sepia *= tone;\n"
    "\n"
    "    colorInput.rgb = lerp(grayscale, sepia, sepiaAmount);\n"
    "\n"
    "    // returning the final color\n"
    "    oColor = colorInput;\n"
    "}\n"
"";

const char* SHADER_SRC_VIGNETTE_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "extern float2 kernelSize;\n"
    "extern float amount;\n"
    "\n"
    "float4 VignettePass(float4 colorInput, float2 tex)\n"
    "{\n"
    "    tex = tex * 2.0 - 1.0;\n"
    "    float dis = length(tex);\n"
    "    dis = dis / 1.4142135623730950488016887242097;\n"
    "    dis = pow(dis, 3.0);\n"
    "    colorInput.rgb *= pow(1.0 - dis * amount, 2.0);\n"
    "    return colorInput;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 c0 = texDiffuse(inTexCoord);\n"
    "    oColor = VignettePass(c0, inTexCoord);\n"
    "}\n"
"";

const char* SHADER_SRC_CARTOON_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "extern float3 tone;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 diffuse = texDiffuse(inTexCoord);\n"
    "    diffuse.rgb = round(diffuse.rgb * tone) / tone;\n"
    "    oColor = diffuse;\n"
    "}\n"
"";

const char* SHADER_SRC_CRT_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "extern float2 screen_size;\n"
    "extern float2 pixel;\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "static float3 filter[8] = {\n"
    "    float3(103.0 / 255.0, 103.0 / 255.0, 24.0 / 255.0), float3(12.0 / 255.0, 34.0 / 255.0, 101.0 / 255.0), float3(64.0 / 255.0, 63.0 / 255.0, 13.0 / 255.0), float3(20.0 / 255.0, 18.0 / 255.0, 63.0 / 255.0),\n"
    "    float3(64.0 / 255.0, 63.0 / 255.0, 13.0 / 255.0), float3(20.0 / 255.0, 18.0 / 255.0, 63.0 / 255.0), float3(103.0 / 255.0, 103.0 / 255.0, 24.0 / 255.0), float3(12.0 / 255.0, 34.0 / 255.0, 101.0 / 255.0)\n"
    "};\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 diffuse = texDiffuse(inTexCoord);\n"
    "    int x = int(inTexCoord.x * screen_size.x) % 4;\n"
    "    int y = int(inTexCoord.y * screen_size.y) % 2;\n"
    "    diffuse.rgb *= filter[y * 4 + x] * 3;\n"
    "    //diffuse.rgb = pow(diffuse.rgb, 0.75);\n"
    "    oColor = diffuse;\n"
    "}\n"
"";

const char* SHADER_SRC_BLURH_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "extern float2 kernelSize;\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 color = float4(0.0, 0.0, 0.0, 0.0);\n"
    "\n"
    "    color += texDiffuse(float2(inTexCoord.x - 6.0 * kernelSize.x, inTexCoord.y)) * 0.002216;\n"
    "    color += texDiffuse(float2(inTexCoord.x - 5.0 * kernelSize.x, inTexCoord.y)) * 0.008764;\n"
    "    color += texDiffuse(float2(inTexCoord.x - 4.0 * kernelSize.x, inTexCoord.y)) * 0.026995;\n"
    "    color += texDiffuse(float2(inTexCoord.x - 3.0 * kernelSize.x, inTexCoord.y)) * 0.064759;\n"
    "    color += texDiffuse(float2(inTexCoord.x - 2.0 * kernelSize.x, inTexCoord.y)) * 0.120985;\n"
    "    color += texDiffuse(float2(inTexCoord.x - 1.0 * kernelSize.x, inTexCoord.y)) * 0.176033;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y)) * 0.199471;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 6.0 * kernelSize.x, inTexCoord.y)) * 0.002216;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 5.0 * kernelSize.x, inTexCoord.y)) * 0.008764;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 4.0 * kernelSize.x, inTexCoord.y)) * 0.026995;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 3.0 * kernelSize.x, inTexCoord.y)) * 0.064759;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 2.0 * kernelSize.x, inTexCoord.y)) * 0.120985;\n"
    "    color += texDiffuse(float2(inTexCoord.x + 1.0 * kernelSize.x, inTexCoord.y)) * 0.176033;\n"
    "\n"
    "    oColor = color;\n"
    "}\n"
"";

const char* SHADER_SRC_BLURV_PS = ""
    "Texture0 texDiffuse {};\n"
    "\n"
    "extern float2 kernelSize;\n"
    "\n"
    "input float2 inTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float4 color = float4(0.0, 0.0, 0.0, 0.0);\n"
    "\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 6.0 * kernelSize.y)) * 0.002216;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 5.0 * kernelSize.y)) * 0.008764;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 4.0 * kernelSize.y)) * 0.026995;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 3.0 * kernelSize.y)) * 0.064759;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 2.0 * kernelSize.y)) * 0.120985;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y - 1.0 * kernelSize.y)) * 0.176033;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y)) * 0.199471;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 6.0 * kernelSize.y)) * 0.002216;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 5.0 * kernelSize.y)) * 0.008764;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 4.0 * kernelSize.y)) * 0.026995;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 3.0 * kernelSize.y)) * 0.064759;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 2.0 * kernelSize.y)) * 0.120985;\n"
    "    color += texDiffuse(float2(inTexCoord.x, inTexCoord.y + 1.0 * kernelSize.y)) * 0.176033;\n"
    "\n"
    "    oColor = color;\n"
    "}\n"
"";
