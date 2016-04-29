#include "OVR_CAPI_GL.h"

#include "openvr.h"
#include <GL/glew.h>

#include "REV_Assert.h"
#include "REV_Common.h"

GLboolean glewInitialized = GL_FALSE;
GLuint revFramebuffer = 0;

GLenum REV_GlewInit()
{
	if (!glewInitialized)
	{
		glewExperimental = GL_TRUE;
		GLenum nGlewError = glewInit();
		if (nGlewError != GLEW_OK)
			return nGlewError;
		glGetError(); // to clear the error caused deep in GLEW
		glewInitialized = GL_TRUE;
		glGenFramebuffers(1, &revFramebuffer);
	}
	return GLEW_OK;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_CreateTextureSwapChainGL(ovrSession session,
                                                            const ovrTextureSwapChainDesc* desc,
                                                            ovrTextureSwapChain* out_TextureSwapChain)
{
	if (REV_GlewInit() != GLEW_OK)
		return ovrError_RuntimeException;

	if (!desc || !out_TextureSwapChain || desc->Type != ovrTexture_2D)
		return ovrError_InvalidParameter;

	ovrTextureSwapChain swapChain = new ovrTextureSwapChainData();
	swapChain->length = 2;
	swapChain->index = 0;
	swapChain->desc = *desc;

	for (int i = 0; i < swapChain->length; i++)
	{
		swapChain->texture[i].eType = vr::API_OpenGL;
		swapChain->texture[i].eColorSpace = vr::ColorSpace_Auto; // TODO: Set this from the texture format.
		glGenTextures(1, (GLuint*)&swapChain->texture[i].handle);
		glBindTexture(GL_TEXTURE_2D, (GLuint)swapChain->texture[i].handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	swapChain->current = swapChain->texture[swapChain->index];

	// Clean up and return
	*out_TextureSwapChain = swapChain;
	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_GetTextureSwapChainBufferGL(ovrSession session,
                                                               ovrTextureSwapChain chain,
                                                               int index,
                                                               unsigned int* out_TexId)
{
	*out_TexId = (GLuint)chain->texture[index].handle;
	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_CreateMirrorTextureGL(ovrSession session,
                                                         const ovrMirrorTextureDesc* desc,
                                                         ovrMirrorTexture* out_MirrorTexture)
{
	if (REV_GlewInit() != GLEW_OK)
		return ovrError_RuntimeException;

	if (!desc || !out_MirrorTexture)
		return ovrError_InvalidParameter;

	ovrMirrorTexture mirrorTexture = new ovrMirrorTextureData();
	mirrorTexture->texture.eType = vr::API_OpenGL;
	mirrorTexture->texture.eColorSpace = vr::ColorSpace_Auto; // TODO: Set this from the texture format.
	glGenTextures(1, (GLuint*)&mirrorTexture->texture.handle);
	glBindTexture(GL_TEXTURE_2D, (GLuint)mirrorTexture->texture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// Clean up and return
	*out_MirrorTexture = mirrorTexture;
	return ovrSuccess;
}

OVR_PUBLIC_FUNCTION(ovrResult) ovr_GetMirrorTextureBufferGL(ovrSession session,
                                                            ovrMirrorTexture mirrorTexture,
                                                            unsigned int* out_TexId)
{
	glBindTexture(GL_TEXTURE_2D, (GLuint)mirrorTexture->texture.handle);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, revFramebuffer);

	int perEyeWidth = mirrorTexture->desc.Width / 2;
	for (int i = 0; i < ovrEye_Count; i++)
	{
		ovrTextureSwapChain chain = session->ColorTexture[i];
		if (chain)
		{
			// TODO: Support texture bounds.
			glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLuint)chain->current.handle, 0);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, perEyeWidth * i, 0, 0, 0, chain->desc.Width, chain->desc.Height);
		}
	}

	*out_TexId = (GLuint)mirrorTexture->texture.handle;
	return ovrSuccess;
}
