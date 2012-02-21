﻿using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Renderer functionality; TODO
	/// </summary>
	public class Renderer
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static Camera _GetViewCamera();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetViewCamera(Camera cam);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetWidth();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetHeight();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _LoadTexture(string path);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1, float z = 1);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _CreateRenderTarget(int width, int height, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DestroyRenderTarget(int id);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetRenderTarget(int id);

		/// <summary>
		/// The camera that the engine is currently using.
		/// </summary>
		public static Camera Camera { get { return _GetViewCamera(); } set { _SetViewCamera(value); } }

		/// <summary>
		/// The width of the screen in pixels.
		/// </summary>
		public static int ScreenHeight { get { return _GetHeight(); } }

		/// <summary>
		/// The height of the screen in pixels.
		/// </summary>
		public static int ScreenWidth { get { return _GetWidth(); } }

		/// <summary>
		/// Loads a texture into memory and generates its ID.
		/// </summary>
		/// <param name="texturePath">The path to the texture.</param>
		/// <returns></returns>
		public static int LoadTexture(string texturePath)
		{
			return _LoadTexture(texturePath);
		}

		/// <summary>
		/// Draws a texture to the screen for one frame.
		/// </summary>
		/// <param name="xPos">The x position on the screen, relative to the top-left corner.</param>
		/// <param name="yPos">The y position on the screen, relative to the top-left corner.</param>
		/// <param name="width">The width to render the texture at.</param>
		/// <param name="height">The height to render the texture at.</param>
		/// <param name="textureId">The ID of the texture (obtained using Renderer.LoadTexture(path)).</param>
		public static void DrawTexture(int xPos, int yPos, int width, int height, int textureId)
		{
			_DrawTextureToScreen(xPos, yPos, width, height, textureId);
		}

		public static int CreateRenderTarget(int width, int height, int flags = 0)
		{
			return _CreateRenderTarget(width, height, flags);
		}

		public static void SetRenderTarget(int id)
		{
			_SetRenderTarget(id);
		}

		public static void DestroyRenderTarget(int id)
		{
			_DestroyRenderTarget(id);
		}
	}

	public class Camera
	{
		public Camera() { }

		public Vec3 ViewDir;
		public Vec3 Position;

		public float FieldOfView;
	}
}
