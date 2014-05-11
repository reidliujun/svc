package test.svc;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Toast;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;

public class Main extends Activity {

	public UIhandler UIupdater = new UIhandler();

	public Bitmap[][] updateBitmap;
	public int BitmapFilledCount[] = new int[2];
	public int bitmapElementNumber = 16;
	public int fps_sample_period = 5;
	
	public ShowView updateView;
	public SVChandler SVC_Handler;
	public TextView FPSmonitor;
	
	/* get all file from /sdcard/svc/files */
	protected String video_file_path = "/sdcard/svc/files";
	File file = new File(video_file_path);
	File[] files = file.listFiles();
	
	String file_name [];	// storage file name
	int file_index [];		// storage file index
	
	// public String fileArray[] =
	// {"/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264",
	// };
	// public String fileArray[] = {
	// "/sdcard/svc/files/juex_t3_l1.264","/sdcard/video_3.264",
	// "/sdcard/sport.264"};
	public String fileArray[] = { "/sdcard/svc/files/doc-reality.264" };
	public String curFilePath = "";
	public int perm[][] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 }, { 2, 0 }, { 2, 1 }, { 1, 2 }, { 2, 2 }, { 0, 2 } };
	public int permID;
	public int curLayerID = 0; 	// from 1 to 32
	public int curTemporalID = 0; 	// from 0 to ?
	public int curThreadNum = 0;

	/* load our native library */
	static {
		System.loadLibrary("svc");
	}
	
	/* native function */
	private static native int decodeSVC(Bitmap[] bitmapArray, int bitmapNum);
	private static native int getWidth();
	private static native int getHeight();
	private static native void setFilePath(String filePath);
	private static native void setLayerID(int layerID);
	private static native void setTemporalID(int temporalID);
	private static native void setThreadNum(int threadNum);
	private static native void clear();

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		/* windows setting */
		requestWindowFeature( Window.FEATURE_NO_TITLE ); // 去除標題
		getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN ); // 全螢幕
		
		/* Layout setting*/
//		RelativeLayout MainLayout = new RelativeLayout( this.getApplicationContext() );
//		setContentView( MainLayout );
		LinearLayout MainLayout = new LinearLayout( this.getApplicationContext() );
		MainLayout.setLayoutParams( new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT) );
		MainLayout.setOrientation(LinearLayout.VERTICAL);  
//		MainLayout.setGravity(Gravity.CENTER);
		this.setContentView( MainLayout );
		this.registerForContextMenu(MainLayout); // register Context Menu
		
		// setContentView(new VideoView(this));
		Init();
		MainLayout.addView(FPSmonitor);
		MainLayout.addView(updateView);
		
		/* Files Name ListView */
		ArrayList<String> file_array = new ArrayList<String>();
		for(File curr_fileName : files) {
			file_array.add(curr_fileName.getName());
		}
		file_name = new String[file_array.size()];
		file_index = new int[file_array.size()];
		for (int i = 0; i < file_array.size(); i++) {
			file_name[i] = file_array.get(i);
			file_index[i] = i;
		}
		
		// UIupdater.curProgress = -1;
		curFilePath = "/sdcard/svc/files/doc-reality.264";
		// curLayerID = 32;
		curLayerID = 32;
		// curTemporalID = 4;
		curTemporalID = 30;
		// curThreadNum = 3;
		curThreadNum = 4;
		
		// ///////////////////////////SVC Config//////////////////////////////
		// setFilePath("/sdcard/sport.264");
		// setLayerID(32);
		// setTemporalID(32);
		// setThreadNum(3);
		// ///////////////////////////////////////////////////////////////////
		
		Thread SVC_Thread = new Thread() {
			@Override
			public void run() {
				Looper.prepare();
				SVC_Handler = new SVChandler();
				UIupdater.obtainMessage(ThreadSignal.FINISH).sendToTarget();
				Looper.loop();
			}
		};
		SVC_Thread.start();
	}
	
	
	/* Menu Settings */
	/* There are operations */
	protected static final int INC_L = Menu.FIRST;
	protected static final int DEC_L = Menu.FIRST+1;
	protected static final int INC_T = Menu.FIRST+2;
	protected static final int DEC_T = Menu.FIRST+3;
	protected static final int INC_C = Menu.FIRST+4;
	protected static final int DEC_C = Menu.FIRST+5;
	
	/*
	 *  Clike long time will show enable file name
	 *  this is called Context Menu 
	 */
	@Override
	public void onCreateContextMenu( ContextMenu contextMenu, View view, ContextMenuInfo menuInfo ) {
		contextMenu.setHeaderTitle("File list");  
		for (int i = 0; i < file_name.length; i++) {
			contextMenu.add(0, file_index[i], 0, file_name[i]);
		}
		super.onCreateContextMenu(contextMenu, view, menuInfo);
		// do-nothing
	}
	
	@Override
	public boolean onContextItemSelected(MenuItem item) {
		for (int i = 0; i < file_index.length; i++) {
			if (item.getItemId() == file_index[i]) {
				Toast.makeText(getApplicationContext(), "file name: " + file_name[i], Toast.LENGTH_LONG).show();
			}
		}
		return super.onContextItemSelected(item);
	}
	/* End Context Menu */
	
	/*
	 * Click the menu item will show the operations
	 * this is called Options Menu
	 */
	@Override 
	public boolean onCreateOptionsMenu( Menu menu) {
		menu.add(0, INC_L, 0, "+L");
		menu.add(0, DEC_L, 0, "-L");
		menu.add(0, INC_T, 0, "+T");
		menu.add(0, DEC_T, 0, "-T");
		menu.add(0, INC_C, 0, "+C");
		menu.add(0, DEC_C, 0, "-C");
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onOptionsItemSelected( MenuItem item ) {
		// Toast.makeText(getApplicationContext(), "Operation_ID: " + item.getItemId(), Toast.LENGTH_LONG).show();
		switch (item.getItemId()) {
		
		case INC_L :
			curLayerID += 5;
			setLayerID(curLayerID);
			Toast.makeText(getApplicationContext(), "Current Layer ID = " + curLayerID, Toast.LENGTH_LONG).show();
			break;
			
		case DEC_L :
			if (curLayerID >= 5) {
				curLayerID -= 5;
				setLayerID(curLayerID);
			}
			Toast.makeText(getApplicationContext(), "Current Layer ID = " + curLayerID, Toast.LENGTH_LONG).show();
			break;
			
		case INC_T :
			curTemporalID += 5; 
			setTemporalID(curTemporalID);
			Toast.makeText(getApplicationContext(), "Current Tempora ID = " + curTemporalID, Toast.LENGTH_LONG).show();
			break;
			
		case DEC_T :
			if (curTemporalID >= 5) {
				curTemporalID -= 5;
				setTemporalID(curTemporalID);
			}
			Toast.makeText(getApplicationContext(), "Current Tempora ID = " + curTemporalID, Toast.LENGTH_LONG).show();
			break;
			
		case INC_C :
			break;
			
		case DEC_C :
			break;
		}
		
		return super.onOptionsItemSelected( item );
	}
	/* End Options Menu */
	/* End Menu Settings */
	
	/* Set Layout for test, it's not used */
	private void setLayout(LinearLayout ll) {
		Button test_btn = new Button(this);
		test_btn.setLayoutParams( new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT) );
		test_btn.setText("123");
		
		LinearLayout bottom_ll = new LinearLayout(this);
		bottom_ll.setOrientation(LinearLayout.HORIZONTAL);
		bottom_ll.setLayoutParams( new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT) );
		
		LinearLayout buttons = new LinearLayout(this);
		buttons.setOrientation(LinearLayout.VERTICAL);
		buttons.setLayoutParams( new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT) );
		
		buttons.addView(test_btn);
		bottom_ll.addView(buttons);
		ll.addView(bottom_ll);
	}
	
	private void Init() {
		updateBitmap = new Bitmap[2][];
		updateBitmap[0] = new Bitmap[bitmapElementNumber];
		updateBitmap[1] = new Bitmap[bitmapElementNumber];
		
		AllocBitmap(1, 1); // allocation two buffers of bitmapElementNumber of one x one bitmaps.
		
		updateView = new ShowView( this.getApplicationContext() );
		RelativeLayout.LayoutParams lp_updateView = new RelativeLayout.LayoutParams( LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT );
		// lp_updateView.topMargin = 51;
		updateView.setLayoutParams(lp_updateView);
		updateView.Init(updateBitmap, BitmapFilledCount);
		
		FPSmonitor = new TextView(this.getApplicationContext());
		FPSmonitor.setTextSize(18);
		FPSmonitor.setBackgroundColor(Color.BLUE);
		RelativeLayout.LayoutParams lp_FPSmonitor = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
		// lp_FPSmonitor.topMargin = 400;
		FPSmonitor.setLayoutParams(lp_FPSmonitor);
	}

	private void AllocBitmap(int W, int H) {
		for (int i = 0; i < bitmapElementNumber; ++i) {
			if (updateBitmap[0][i] != null)
				updateBitmap[0][i].recycle();
			
			if (updateBitmap[1][i] != null)
				updateBitmap[1][i].recycle();
			
			updateBitmap[0][i] = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
			updateBitmap[1][i] = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
		}
	}

	class ThreadSignal {
		public static final int UPDATE_BUFFER_0 = 0x0000;
		public static final int UPDATE_BUFFER_1 = 0x0001;
		public static final int WRITE_BUFFER_0 = 0x0010;
		public static final int WRITE_BUFFER_1 = 0x0011;
		public static final int FINISH = 0x1111;
	}
	
	
	/* UI Handler */
	class UIhandler extends Handler {
		public int curProgress;
		private boolean startConter;
		private int counter, totalFrame;
		private double time_start, time_end, time_orig, cum_value;

		public void Init() {
			totalFrame = 0;
			counter = 0;
			time_start = System.currentTimeMillis();
			time_orig = System.currentTimeMillis();
			startConter = false;
		}

		public UIhandler() {
			Init();
		}

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case ThreadSignal.UPDATE_BUFFER_0:
				SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_1).sendToTarget();
				updateView.Update(0);
				counter = BitmapFilledCount[0];
				break;
				
			case ThreadSignal.UPDATE_BUFFER_1:
				SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_0).sendToTarget();
				updateView.Update(1);
				counter = BitmapFilledCount[1];
				break;
				
			case ThreadSignal.FINISH:
				this.removeMessages(ThreadSignal.UPDATE_BUFFER_1);
				this.removeMessages(ThreadSignal.UPDATE_BUFFER_0);
				SVC_Handler.removeMessages(ThreadSignal.WRITE_BUFFER_0);
				SVC_Handler.removeMessages(ThreadSignal.WRITE_BUFFER_1);
				/*
				 * String res = String.valueOf(curProgress) + " " + curFilePath
				 * + " " + String.valueOf(curLayerID) + " " +
				 * String.valueOf(curTemporalID) + " " +
				 * String.valueOf(curThreadNum) + " " +
				 * String.valueOf(cum_value); if(curProgress!=-1)
				 * appendLog(res);
				 */
				setFilePath(curFilePath);
				setLayerID(curLayerID);
				setTemporalID(curTemporalID);
				setThreadNum(curThreadNum);
				Init();
				SVC_Handler.Init();
				SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_0).sendToTarget();
				break;
			}
			
			if (counter >= 0) {
				time_end = System.currentTimeMillis();
				totalFrame += counter;
				// double
				// val=((double)fps_sample_period*1000.0)/(time_end-time_start);
				double val = ((double) counter * 1000.0) / (time_end - time_start);
				cum_value = ((double) totalFrame * 1000.0) / (time_end - time_orig);
				
				time_start = System.currentTimeMillis();
				
				FPSmonitor.setText("Running ID : "
						+ String.valueOf(curProgress) + " , File : "
						+ curFilePath + ", Thread : "
						+ String.valueOf(curThreadNum) + "\n" + "Current : "
						+ String.valueOf(val) + " , Cumulative : "
						+ String.valueOf(cum_value));
				FPSmonitor.invalidate();
				
				if (!startConter && totalFrame > 50) {
					startConter = true;
					totalFrame = 0;
					time_orig = System.currentTimeMillis();
				}
			}
			super.handleMessage(msg);
		}
	}
	/* End UIhandler */

	/* SVC handler */
	class SVChandler extends Handler {
		private boolean alerted;

		public void Init() {
			alerted = false;
		}

		public SVChandler() {
			Init();
		}

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case ThreadSignal.WRITE_BUFFER_0:
				BitmapFilledCount[0] = decodeSVC(updateBitmap[0], bitmapElementNumber);
				
				if (BitmapFilledCount[0] >= 0) {
					UIupdater.obtainMessage(ThreadSignal.UPDATE_BUFFER_0).sendToTarget();
				} else if (BitmapFilledCount[0] == -1) {
					int W = getWidth();
					int H = getHeight();
					// Log.d("*****",String.valueOf(W)+" "+String.valueOf(H));
					AllocBitmap(W, H);
					SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_0).sendToTarget();
				}
				break;
				
			case ThreadSignal.WRITE_BUFFER_1:
				BitmapFilledCount[1] = decodeSVC(updateBitmap[1], bitmapElementNumber);
				
				if (BitmapFilledCount[1] >= 0) {
					UIupdater.obtainMessage(ThreadSignal.UPDATE_BUFFER_1).sendToTarget();
				} else if ( BitmapFilledCount[1] == -1 ) {
					int W = getWidth();
					int H = getHeight();
					// Log.d("*****",String.valueOf(W)+" "+String.valueOf(H));
					AllocBitmap(W, H);
					SVC_Handler.obtainMessage( ThreadSignal.WRITE_BUFFER_1 ).sendToTarget();
				}
				break;
			}
		}
	}
	/* End SVC handler */

	public void appendLog(String text) {
		File logFile = new File("sdcard/svc/svc_log.txt");
		if (!logFile.exists()) {
			try {
				logFile.createNewFile();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		try {
			BufferedWriter buf = new BufferedWriter( new FileWriter( logFile, true ) );
			buf.append(text);
			buf.newLine();
			buf.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}


/* Customized SurfaceView called ShowView */
class ShowView extends SurfaceView {
	private Bitmap updateBitmap[][];
	private int BitmapFilledCount[];
	private int current_buffer_index;
	private int counter = 0;
	private Canvas canvas;
	private SurfaceHolder viewHolder;

	public ShowView(Context context) {
		super(context);
		viewHolder = this.getHolder();
	}

	public void Init(Bitmap[][] bitmapSrc, int BitmapFilledCountSrc[]) {
		updateBitmap = bitmapSrc;
		BitmapFilledCount = BitmapFilledCountSrc;
		current_buffer_index = 0;
	}

	public void Update(int buffer_index) {
		current_buffer_index = buffer_index;
		for (int i = 0; i < BitmapFilledCount[current_buffer_index]; ++i) {
			canvas = viewHolder.lockCanvas();
			canvas.drawBitmap(updateBitmap[current_buffer_index][i], 0, 0, null);
			viewHolder.unlockCanvasAndPost(canvas);
			/*
			 * try { Thread.sleep(33); } catch (InterruptedException e) {
			 * e.printStackTrace(); }
			 */
		}
	}

}
/* End ShowView */
