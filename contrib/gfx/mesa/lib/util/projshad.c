/* projshad.c */


/*
 * Generate the projection matrix for a projective shadow
 * Borrowed from GLUT distribution's scube demo, written
 * by David Yu.
 *
 * This code is freely distributable.
 *
 */


/*
 * ground is the equation of the plane onto which the shadow should
 * be projected.
 * light is the homogenious coordinate of the light position.
 */
static void
myShadowMatrix(float ground[4], float light[4])
{
  float dot;
  float shadowMat[4][4];

  dot = ground[0] * light[0] +
    ground[1] * light[1] +
    ground[2] * light[2] +
    ground[3] * light[3];

  shadowMat[0][0] = dot - light[0] * ground[0];
  shadowMat[1][0] = 0.0 - light[0] * ground[1];
  shadowMat[2][0] = 0.0 - light[0] * ground[2];
  shadowMat[3][0] = 0.0 - light[0] * ground[3];

  shadowMat[0][1] = 0.0 - light[1] * ground[0];
  shadowMat[1][1] = dot - light[1] * ground[1];
  shadowMat[2][1] = 0.0 - light[1] * ground[2];
  shadowMat[3][1] = 0.0 - light[1] * ground[3];

  shadowMat[0][2] = 0.0 - light[2] * ground[0];
  shadowMat[1][2] = 0.0 - light[2] * ground[1];
  shadowMat[2][2] = dot - light[2] * ground[2];
  shadowMat[3][2] = 0.0 - light[2] * ground[3];

  shadowMat[0][3] = 0.0 - light[3] * ground[0];
  shadowMat[1][3] = 0.0 - light[3] * ground[1];
  shadowMat[2][3] = 0.0 - light[3] * ground[2];
  shadowMat[3][3] = dot - light[3] * ground[3];

  glMultMatrixf((const GLfloat *) shadowMat);
}
