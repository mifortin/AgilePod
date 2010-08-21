/*
 *  TextureManager.h
 *  GravitySpace
 */

static GLuint g_appTexture		= 0;
static GLuint g_miniTexture		= 0;

void SetAppTexture(GLuint in_appTexture)
{
	g_appTexture = in_appTexture;
}


void SetMiniTexture(GLuint in_miniTexture)
{
	g_miniTexture = in_miniTexture;
}


static void useAppTexture()
{
	glBindTexture(GL_TEXTURE_2D, g_appTexture);
}

static void useMiniTexture()
{
	glBindTexture(GL_TEXTURE_2D, g_miniTexture);
}
