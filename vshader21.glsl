uniform mat4 trans;
attribute vec4 vPosition;
attribute vec4 vColor;
varying vec4 color;

void
main()
{
    gl_Position = trans*vPosition;
    color = vColor;
}
