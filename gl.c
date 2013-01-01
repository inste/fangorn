/*
 * gl.c
 *
 *  Created on: 08.12.2012
 *      Author: ilya
 */

#include "gl.h"

// System scale, computed in do_prerender() by size of vertex object
static double scale = 1.0;

// System offsets, computed in do_prerender() by size of vertex object
static double zx = 0, zy = 0;

// Userscale can be changed by pressing PgUp / PgDown
static double userscale = 1.0;

// Useroffset, can be changed by pressing Left / Right / Up / Down
static double ux = 0, uy = 0;

// Userangle, can be changed by pressing Home / End
static double ua = 0;

extern struct RunTime RT;

void do_prerender(GLfloat * Vertex, GLfloat * Colour) {

	int i;
	struct Context ctx;
	double newangle = 0;
	double mx, my, mix, miy;

	mx = my = mix = miy = 0;
	ctx.posx = 0.0;
	ctx.posy = 0.0;
	ctx.angle = PI / 2;

	for (i = 0; i < RT.size; ++i) {

		// Upchar should be rendered
		if (T_UPCHAR == RT.line[i].type) {
			Colour[6 * i] = Colour[6 * i + 3] = 1 - RT.line[i].dctx.r;
	    	Colour[6 * i + 1] = Colour[6 * i + 4] = 1 - RT.line[i].dctx.g;
	    	Colour[6 * i + 2] = Colour[6 * i + 5] = 1 - RT.line[i].dctx.b;

	    	if (fabs(newangle) < 0.001) {
	    		Vertex[4 * i] = ctx.posx;
	    		Vertex[4 * i + 1] = ctx.posy;
	    		Vertex[4 * i + 2] = ctx.posx = (ctx.posx + L * cos(ctx.angle));
	    			 Vertex[4 * i + 3] = ctx.posy = (ctx.posy + L * sin(ctx.angle));
	    		 } else {
	    			 Vertex[4 * i] = ctx.posx;
	    			 Vertex[4 * i + 1] = ctx.posy;
	    			 Vertex[4 * i + 2] = ctx.posx = (ctx.posx + L * cos(ctx.angle + newangle));
	    			 Vertex[4 * i + 3] = ctx.posy = (ctx.posy + L * sin(ctx.angle + newangle));
	    			 ctx.angle += newangle;
	    			 newangle = 0;
	    		 }
	    	 }

			 // Lochar controls only evolution of the curve
	    	 if (T_LOCHAR == RT.line[i].type) {
	    		 if (fabs(newangle) < 0.001) {
	    			 ctx.posx += L * cos(ctx.angle);
	    			 ctx.posy += L * sin(ctx.angle);
	    		 } else {
	    			 ctx.posx += L * cos(ctx.angle + newangle);
	    			 ctx.posy += L * sin(ctx.angle + newangle);
	    			 ctx.angle += newangle;
	    			 newangle = 0;
	    		 }
	    	 }

	    	 if (T_PLUS == RT.line[i].type) {
	    		 newangle =  RT.line[i].value * PI / 180;
	    	 }
	    	 if (T_MINUS == RT.line[i].type) {
	    		 newangle = -RT.line[i].value * PI / 180;
	    	 }

	    	 if (T_PUSH == RT.line[i].type) {
	    		 push(ctx);
	    	 }

	    	 if (T_POP == RT.line[i].type) {
	    		 ctx = pop();
	    	 }

	    	 if (ctx.posx > mx)
	    		 mx = ctx.posx;
	    	 if (ctx.posy > my)
	    		 my = ctx.posy;

	    	 if (ctx.posx < mix)
	    		 mix = ctx.posx;
	    	 if (ctx.posy < miy)
	    		 miy = ctx.posy;
	     }

	  scale = ((mx - mix) > (my - miy)) ? mx - mix : my - miy;
	  zx = -((mx + mix) / (scale * 1.4)) / 2;
	  zy = -((my + miy) / (scale * 1.4)) / 2;
}

void do_render()
{
	GLfloat * Vertex;
	GLfloat * Colour;

    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonOffset(1, 1);
    glEnable(GL_DEPTH_TEST);

    Vertex = (GLfloat *)malloc(RT.size * 2 * 2 * sizeof(GLfloat));
    Colour = (GLfloat *)malloc(RT.size * 2 * 3 * sizeof(GLfloat));

	do_prerender(Vertex, Colour);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(ux + zx, uy + zy, 0);
    glRotated(ua, 0, 0, 1);
    glScaled(1/(scale * 1.4 * userscale), 1/(scale * 1.4 * userscale), 1);

    glVertexPointer(2, GL_FLOAT, 0, Vertex);
    glColorPointer(3, GL_FLOAT, 0, Colour);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_LINES, 0, 2 * RT.size);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glFlush();
    glutSwapBuffers();

    printf("Drawed %d vertices\n", RT.size * 2);

}

void do_press(int cb, int x, int y) {
	switch (cb) {
		case GLUT_KEY_PAGE_UP :
			userscale /= 1.5;
			break;
		case GLUT_KEY_PAGE_DOWN :
			userscale *= 1.5;
			break;
		case GLUT_KEY_LEFT:
			ux += userscale * 0.15;
			break;
		case GLUT_KEY_RIGHT:
			ux -= userscale * 0.15;
			break;
		case GLUT_KEY_UP:
			uy -= userscale * 0.15;
			break;
		case GLUT_KEY_DOWN:
			uy += userscale * 0.15;
			break;
		case GLUT_KEY_HOME:
			ua += PI / 2;
			break;
		case GLUT_KEY_END:
			ua -= PI / 2;
			break;
	}
	glutPostRedisplay();
}


void start_gl_render(int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Fangorn by Ilya Poneyaev");
    glClearColor(0.99, 1.0, 0.98, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, -1.0, 1.0);
    glutDisplayFunc(do_render);
    glutSpecialFunc(do_press);
    glutMainLoop();
}
