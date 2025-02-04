#include "raylib.h"
#include <cmath> // Used for abs()

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - heightmap loading and drawing");

    // Define our custom camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = Vector3{ 0.0f, 12.0f, 16.0f };     // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };          // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type

    Vector3 playerPosition = { 0.0f, 0.5f, 8.0f };
    Vector3 playerSize = { 1.0f, 1.0f, 1.0f };
    float playerCentreSize = 0.25f;
    Color playerColor = { 0, 255, 0, 63 };

    // Image image = LoadImage("resources/heightmap.png");     // Load heightmap image (RAM) (ORIGINAL)
    // Image image = LoadImage("resources/gridCanyonUnlabled32.png"); // Test map while developing collision
    Image image = LoadImage("resources/heightmapWider.png"); // Canyon map from David

    Texture2D texture = LoadTextureFromImage(image);        // Convert image to texture (VRAM)

    Vector3 mapSize = { 16, 8, 16 };
    Mesh mesh = GenMeshHeightmap(image, mapSize); // Generate heightmap mesh (RAM and VRAM)
    Model model = LoadModelFromMesh(mesh);                  // Load model from generated mesh

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    Vector3 mapPosition = { -8.0f, 0.0f, -8.0f };           // Define model position

    // RS: CAN'T UnloadImage HERE, NEED TO REF IMAGE, SHIFTED TO DE-INIT
    // UnloadImage(image);             // Unload heightmap image from RAM, already uploaded to VRAM.

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_PERSPECTIVE);

        // Move player
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 0.2f;
        else if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 0.2f;
        else if (IsKeyDown(KEY_DOWN)) playerPosition.z += 0.2f;
        else if (IsKeyDown(KEY_UP)) playerPosition.z -= 0.2f;

        // Get Normalised Coord
        float worldNormalX = (playerPosition.x + abs(mapPosition.x)) / mapSize.x;
        float worldNormalZ = (playerPosition.z + abs(mapPosition.z)) / mapSize.z;
        float texUcoord = worldNormalX * texture.width;
        float texVcoord = worldNormalZ * texture.height;

        // Clampity clamp (make this a helper function?) 0.001f - just to be sure we don't get OOBounds error
        if (texUcoord > texture.height - 0.001f) texUcoord = texture.height - 0.001f;
        if (texUcoord < 0) texUcoord = 0;
        
        if (texVcoord > texture.width - 0.001f) texVcoord = texture.width - 0.001f;
        if (texVcoord < 0) texVcoord = 0;

        Color colorFromPosition = GetImageColor(image, texUcoord, texVcoord);
        float worldYNormalFromCol = colorFromPosition.r / 255.0f;
        float worldYPos = worldYNormalFromCol * mapSize.y;

        playerPosition.y = worldYPos;

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

            // Draw player
            DrawSphere(playerPosition, playerCentreSize * 0.5f, RED);

            // Draw terrain
            DrawModel(model, mapPosition, 1.0f, GRAY);

            // Draw BG
            DrawGrid(20, 1.0f);

        EndMode3D();

        DrawText((TextFormat("XPos: %f, YPos: %f, ZPos: %f", playerPosition.x, playerPosition.y, playerPosition.z)), 10, 10, 32, GREEN);
        DrawText((TextFormat("NormalX: %f, NormalZ: %f", worldNormalX, worldNormalZ)), 10, 45, 32, ORANGE);
        DrawText((TextFormat("TexU: %f, TexV: %f", texUcoord, texVcoord)), 10, 90, 32, PURPLE);
        DrawText((TextFormat("World Y Normal: %f", worldYNormalFromCol)), 10, 135, 32, BROWN);
        DrawText((TextFormat("World Y Pos: %f", worldYPos)), 10, 170, 32, SKYBLUE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadImage(image);             // Unload heightmap image from RAM, already uploaded to VRAM
    UnloadTexture(texture);     // Unload texture
    UnloadModel(model);         // Unload model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}