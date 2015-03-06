package com.orca.orca3denginelibrarymodule;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class Orca3DEngineView extends GLSurfaceView {

	public Orca3DEngineView(Context context) {
		super(context);
        Log.i("GL2JNIView", "context1 ...........");
//		setEGLConfigChooser(5, 6, 5, 0, 8, 8);
		setEGLConfigChooser(8, 8, 8, 0, 16, 8);
        setEGLContextClientVersion(2);
        setEGLWindowSurfaceFactory(new SurfaceFactory());
        setRenderer(new Renderer());
	}
	
	public Orca3DEngineView(Context context, AttributeSet attrs) {
	    super(context, attrs);
        Log.i("GL2JNIView", "context2 ...........");
//		setEGLConfigChooser(5, 6, 5, 8, 16, 0);
    	setEGLConfigChooser(8, 8, 8, 8, 24, 0);
//        setEGLConfigChooser(true);

//        setEGLContextClientVersion(2);
        setEGLWindowSurfaceFactory(new SurfaceFactory());


        setRenderer(new Renderer());

	}

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // GL2JNILib.nativeSetSurface(holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder) {
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        // GL2JNILib.nativeSetSurface(null);
    }
    
    private static class SurfaceFactory implements EGLWindowSurfaceFactory {
		@Override
		public javax.microedition.khronos.egl.EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display, EGLConfig config, Object nativeWindow) {
	        Log.i("GL2JNIView", "createWindowSurface ...........");

			// this is a bit of a hack to work around Droid init problems - if you
            // don't have this, it'll get hung up on orientation changes
			javax.microedition.khronos.egl.EGLSurface eglSurface = null;


/*			
            while (eglSurface == null) {
                try {
                    eglSurface = egl.eglCreateWindowSurface(display, config, nativeWindow, null);
                } catch (Throwable t) {
                } finally {
                    if (eglSurface == null) {
                        try {
                                Thread.sleep(10);
                        } catch (InterruptedException t) {
                        }
                    }
                }
            }
*/
            return eglSurface;    	
        }
    	
		@Override
		public void destroySurface(EGL10 egl, EGLDisplay display, javax.microedition.khronos.egl.EGLSurface surface) {
            egl.eglDestroySurface(display, surface);
		}
    }
    
    private static class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            // GLES3JNILib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            // GLES3JNILib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // GLES3JNILib.init();
        }
    }
    
}
