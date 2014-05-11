package test.svc;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.text.DecimalFormat;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Debug;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;

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
//	protected String video_file_path = "/sdcard/svc/files";
//	File file = new File(video_file_path);
//	File[] files = file.listFiles();
//	String file_name [];	// storage file name
//	int file_index [];		// storage file index
	
	// public String fileArray[] =
	// {"/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264","/sdcard/svc/files/doc-reality.264",
	// };
	// public String fileArray[] = {
	// "/sdcard/svc/files/juex_t3_l1.264","/sdcard/video_3.264",
	// "/sdcard/sport.264"};
	public String fileArray[] = { "/sdcard/svc/files/doc-reality.264" };
	public String curFilePath = "";
	public String curPrefix = "";
	public int perm[][] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 }, { 2, 0 }, { 2, 1 }, { 1, 2 }, { 2, 2 }, { 0, 2 } };
	public int permID;
	public int curLayerID = 0; 	// from 0 to 32 (0~15, 16~31, 32~47. 48~)
	public int curTemporalID = 0; 	// from 1 to 3
	public int curThreadNum = 0;	// 3 is best for four CPUs phone
	public int curTimes = 0;
	
	/* for experiment */
	BufferedWriter bw;	// writer buffer
	MemoryInfo mem_info;
	android.os.Debug.MemoryInfo[] memInfoArray;
	ActivityManager am;
	
	DecimalFormat df_8 = new DecimalFormat("#0.00000000");
	DecimalFormat df_4 = new DecimalFormat("#0.0000");
	
	long start_time = 0;		// the timeStamp of start this app
	long firstPlay_time = 0;	// the timeStamp of first play the first frame
	boolean checkStartupTime = false; // check weather already write startup time
	long delay_time = 0;		// 
	
	int sampling_rate = 5000; // msec
	int sample_rate_per_god = 0;
	int sampling_counter = 0; // counter the number of sample in sampling_rate
	int total_counter = 0;
	
	float total_fps = 0f;
	long total_heapSize = 0;
	int total_pssSize = 0;
	float total_TP = 0f;	// TP is short for ThroughPut
	
	/* load our native library */
	static {
		System.loadLibrary("svc"); 
	}
	
	/* native function */
	private static native int decodeSVC(Bitmap[] bitmapArray, int bitmapNum);
	private static native int getWidth();
	private static native int getHeight();
	private static native int getLayerID();
	private static native int getTemporalID();
	private static native float getThroughput();
	private static native int getNetworkDelay();
	private static native int getDecodeDelay();
	private static native int getSwitchGOP();
	private static native void setFilePath(String filePath, String prefix);
	private static native void setLayerID(int layerID);
	private static native void setTemporalID(int temporalID);
	private static native void setThreadNum(int threadNum);
	private static native void setStartTime();
	private static native void clear();

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		start_time = System.currentTimeMillis();
		
		/* windows setting */
		requestWindowFeature( Window.FEATURE_NO_TITLE ); // 去除標題
//		getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN ); // 全螢幕
		
		/* Layout setting*/
//		RelativeLayout MainLayout = new RelativeLayout( this.getApplicationContext() );
//		setContentView( MainLayout );
		LinearLayout MainLayout = new LinearLayout( this.getApplicationContext() );
		MainLayout.setLayoutParams( new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT) );
		MainLayout.setOrientation(LinearLayout.VERTICAL);  
//		MainLayout.setGravity(Gravity.CENTER);
		this.setContentView(MainLayout);
		this.registerForContextMenu(MainLayout); // register Context Menu
		
		// setContentView(new VideoView(this));
		Init();
		MainLayout.addView(FPSmonitor);
		MainLayout.addView(updateView);
				
		/* Files Name ListView */
//		ArrayList<String> file_array = new ArrayList<String>();
//		for(File curr_fileName : files) {
//			file_array.add(curr_fileName.getName());
//		}
//		file_name = new String[file_array.size()];
//		file_index = new int[file_array.size()];
//		for (int i = 0; i < file_array.size(); i++) {
//			file_name[i] = file_array.get(i);l
//			file_index[i] = i;
//		}
		setStartTime();
		
		am = (ActivityManager) this.getSystemService(ACTIVITY_SERVICE);
		mem_info = new ActivityManager.MemoryInfo();
//		am.getMemoryInfo(mem_info);
		
//		List<RunningAppProcessInfo> infos = am.getRunningAppProcesses();
//		int pid[] = new int[1];
//		pid[0] = android.os.Process.myPid();
//		memInfoArray = am.getProcessMemoryInfo(pid);
		
		// UIupdater.curProgress = -1;
		curFilePath = "/sdcard/svc/files/doc-reality.264";
		curTemporalID = 3;
		curThreadNum = 3;
		
		
		curPrefix = "jeux"; // doc-reality, jeux, soap, sport, talking_head
		curLayerID = 16; // Layer_1:16, Layer_2:32, Layer_3:33
		boolean with3G = false;
		curTimes = 99;
		sample_rate_per_god = 25;
		
		String _3g = "";
		if (with3G) _3g = "_3G_";
		
		String res = "";
		// L: low resolution, M: medium resolution, H: high resolution
		if (curLayerID == 16) res = "L";
		else if (curLayerID == 32) res = "M";
		else if (curLayerID == 33) res = "H";
		
		// ///////////////////////////SVC Config//////////////////////////////
		// setFilePath("/sdcard/sport.264");
		// setLayerID(32);
		// setTemporalID(32);
		// setThreadNum(3);
		// ///////////////////////////////////////////////////////////////////
		
//		String output_file_name = curPrefix + "_" + "L" + curLayerID + "_T" + curTemporalID + "_Tn"  + curThreadNum + "__" + curTimes;
		String output_file_name = curPrefix + "__" + res + "__" + curTimes + "_S" + sample_rate_per_god + _3g +"_TT";
		setWriterFile(output_file_name);
	
		
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
	
	private void setWriterFile(String fileName) {
		try {
			FileWriter fw = new FileWriter("/sdcard/svc/output/" + fileName + ".txt");
			bw = new BufferedWriter(fw);
		} catch (IOException ioe) {
			System.exit(0);
		}
	}
	
	
	/* Menu Settings */
	/* There are operations */
	protected static final int INC_L = Menu.FIRST;
	protected static final int DEC_L = Menu.FIRST+1;
	protected static final int INC_T = Menu.FIRST+2;
	protected static final int DEC_T = Menu.FIRST+3;

	
	/*
	 *  Clike long time will show enable file name
	 *  this is called Context Menu 
	 */
//	@Override
//	public void onCreateContextMenu( ContextMenu contextMenu, View view, ContextMenuInfo menuInfo ) {
//		contextMenu.setHeaderTitle("File list");  
//		for (int i = 0; i < file_name.length; i++) {
//			contextMenu.add(0, file_index[i], 0, file_name[i]);
//		}
//		super.onCreateContextMenu(contextMenu, view, menuInfo);
//		// do-nothing
//	}
//	
//	@Override
//	public boolean onContextItemSelected(MenuItem item) {
//		for (int i = 0; i < file_index.length; i++) {
//			if (item.getItemId() == file_index[i]) {
//				Toast.makeText(getApplicationContext(), "file name: " + file_name[i], Toast.LENGTH_LONG).show();
//			}
//		}
//		return super.onContextItemSelected(item);
//	}
	/* End Context Menu */
	
	/*
	 * Click the menu item will show the operations
	 * this is called Options Menu
	 */
	@Override 
	public boolean onCreateOptionsMenu( Menu menu) {
		menu.add(0, INC_L, 0, "+ Spatial");
		menu.add(0, DEC_L, 0, "- Spatial");
		menu.add(0, INC_T, 0, "+ Temporal");
		menu.add(0, DEC_T, 0, "- Temporal");
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onOptionsItemSelected( MenuItem item ) {
		switch (item.getItemId()) {
		
		case INC_L :
			if (curLayerID == 16)
			{
				curLayerID = 32;
				setLayerID(curLayerID);
			}
			else if (curLayerID == 32) {
				curLayerID = 33;
				setLayerID(curLayerID);
			}
			break;
			
		case DEC_L :
			if (curLayerID == 33)
			{
				curLayerID = 32;
				setLayerID(curLayerID);
			}
			else if (curLayerID == 32)
			{
				curLayerID = 16;
				setLayerID(curLayerID);
			}
			break;
			
		case INC_T :
			if (curTemporalID < 3)
			{
				curTemporalID++; 
				setTemporalID(curTemporalID);
			}
			break;
			
		case DEC_T :
			if (curTemporalID > 1)
			{
				curTemporalID--;
				setTemporalID(curTemporalID);
			}
			break;
		}
		
		Toast.makeText(
				getApplicationContext(),
				"Current Layer ID = " + curLayerID + "\nCurrent Temporal ID = " + curTemporalID,
				Toast.LENGTH_LONG).show();
		
		return super.onOptionsItemSelected( item );
	}
	/* End Options Menu */
	/* End Menu Settings */
	
	private void Init() {
		updateBitmap = new Bitmap[2][];
		updateBitmap[0] = new Bitmap[bitmapElementNumber];
		updateBitmap[1] = new Bitmap[bitmapElementNumber];
		
		AllocBitmap(1, 1); // allocation two buffers of bitmapElementNumber of one by one bitmaps.
		
		// set surface View
		updateView = new ShowView( this.getApplicationContext() );
		RelativeLayout.LayoutParams lp_updateView = new RelativeLayout.LayoutParams( LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT );
		// lp_updateView.topMargin = 51;
		updateView.setLayoutParams( lp_updateView );
		updateView.Init(updateBitmap, BitmapFilledCount);
		
		// set text
		FPSmonitor = new TextView( this.getApplicationContext() );
		FPSmonitor.setTextSize(18);
		FPSmonitor.setBackgroundColor( Color.BLUE );
		RelativeLayout.LayoutParams lp_FPSmonitor = new RelativeLayout.LayoutParams( LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT );
		// lp_FPSmonitor.topMargin = 400;
		FPSmonitor.setLayoutParams( lp_FPSmonitor );
	}

	private void AllocBitmap(int W, int H) {
		for (int i = 0; i < bitmapElementNumber; ++i) {
			if (updateBitmap[0][i] != null)
				updateBitmap[0][i].recycle();
			
			if (updateBitmap[1][i] != null)
				updateBitmap[1][i].recycle();
			
			Bitmap source = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
			updateBitmap[0][i] = source;
			updateBitmap[1][i] = source;
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
				setThreadNum(curThreadNum);
				setLayerID(curLayerID);
				setTemporalID(curTemporalID);
				setFilePath(curFilePath, curPrefix);
				Init();
				SVC_Handler.Init();
				SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_0).sendToTarget();
				break;
			}
			
			if (counter >= 0) {
				time_end = System.currentTimeMillis();
				totalFrame += counter;
				double val = ((double) counter * 1000.0) / (time_end - time_start);
				cum_value = ((double) totalFrame * 1000.0) / (time_end - time_orig);
				
				time_start = System.currentTimeMillis();
				
				FPSmonitor.setText("Running ID : " + String.valueOf(curProgress) 
//						+ " , File : " + curFilePath 
						+ ", T: " + getTemporalID() + ", L: " + getLayerID()
						+ ", Thread : " + String.valueOf(curThreadNum)
						+ " ---> " + curPrefix + "(" + total_counter + " : " + getSwitchGOP() + ")"
						+ "\n" 
						+ "Current : " + df_8.format(val)
						+ " , Cumulative : " + df_8.format(cum_value));
				FPSmonitor.invalidate();

				am.getMemoryInfo(mem_info);
				memInfoArray = am.getProcessMemoryInfo(pid);
				
				total_counter++;
//				Log.d("COUNTER", "TESTSEG : counter = " + total_counter);
				if (val > 1) {
					total_fps += val;
					total_heapSize += Debug.getNativeHeapAllocatedSize();
					total_pssSize += memInfoArray[0].getTotalPrivateDirty();
					total_TP += getThroughput();
					sampling_counter ++;
				}
				
//				if (((System.currentTimeMillis() - delay_time) > sampling_rate) && (sampling_counter > 0))
				if (total_counter % sample_rate_per_god == 0 || (total_counter == 563))
				{
					doExperiment();
					Log.v("LOG", "TESTSEG : wrote log into file, at GOPs: " + total_counter);
					
					if(total_counter >= 563) {
						Log.e("SVC", "****** EXIT SYSTEM ******");
						System.exit(0);
					}
				}
				
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
	
	private int pid[] = {android.os.Process.myPid()};
	
	// Record all values of experiment.
	// including TransferDelay, DecodeDelay, FPS, and ThroughPut
	private void doExperiment(
//			double fps
			) {
		
		Log.d("PID", "pid = " + pid[0]);
		
		String startup = "";
		if(firstPlay_time > 0) { 
			if (!checkStartupTime) {
				startup = System.currentTimeMillis() + "=> TD:" + getNetworkDelay() + ",DD:"+ getDecodeDelay() + "\n";
				checkStartupTime = true;
			}
		}
		
//		Log.i("TP", "TESTSEG : TP: " + (total_TP/sampling_counter));
		
		String output_line = startup 
				+ System.currentTimeMillis() + "=> "
				+ "FPS:" + df_4.format(total_fps/sampling_counter) + ","
//				+ "Mem Usage:" + df_4.format(((((float)total_heapSize/sampling_counter)/1024.0) + ((float)total_pssSize/sampling_counter))/1024.0) + ","
//				+ "CPU usage:" + df_8.format(readUsage(android.os.Process.myPid())) + ","
				+ "TP:" + df_4.format(total_TP/sampling_counter) + ","
				+ "\n";
		
//		if (fps > 0)
		{
			try {
				bw.write(output_line);
				bw.flush();
			} catch (Exception e) {
				Log.d("IO", "write file: " + e);
				System.exit(0);
			}
		}
		
		// clear all data
		delay_time = System.currentTimeMillis();
		total_fps = 0f;
		total_heapSize = 0;
		total_pssSize = 0;
		total_TP = 0;
		sampling_counter = 0;
	}
	
	boolean first = true;
	long cpu1, cpu2, idle1, idle2;
	/*
	 * calculate total CPU utilization for this APP
	 * but the value has some problem, is always less than 25%
	 * so, we do this experiment later.
	 */
	private float readUsage(int pid) {
		String load = "";
		String[] toks;
		
		try {
			RandomAccessFile reader = new RandomAccessFile("/proc/" + pid + "/stat", "r");
			
			/*
			 * toks value detail:
			 * http://blog.csdn.net/pppjob/article/details/4060336
			 */
			/*
			if (first) {
				load = reader.readLine();
				toks = load.split("\\s+");
//				idle1 = Long.parseLong(toks[4]);
//				cpu1 = Long.parseLong(toks[1]) + Long.parseLong(toks[2]) + Long.parseLong(toks[3])
//						+ Long.parseLong(toks[5]) + Long.parseLong(toks[6]) + Long.parseLong(toks[7]);
				cpu1 = Long.parseLong(toks[11]) + Long.parseLong(toks[12]);
				first = false;
			}
			else {
//				idle1 = idle2;
				cpu1 = cpu2;
			}
			*/
			
//			reader.seek(0);
			load = reader.readLine();
			reader.close();
			
			toks = load.split("\\s+");
			
//			idle2 = Long.parseLong(toks[4]);
//			cpu2 = Long.parseLong(toks[1]) + Long.parseLong(toks[2]) + Long.parseLong(toks[3])
//					+ Long.parseLong(toks[5]) + Long.parseLong(toks[6]) + Long.parseLong(toks[7]);
			cpu2 = Long.parseLong(toks[13]) + Long.parseLong(toks[14]);
			Log.d("CPU", "toks[11]: " + toks[13] + ", toks[12]: " + toks[14]);
			
//			if ((cpu2 - cpu1 == 0) && (idle2 - idle1 == 0)) {
//			if(cpu2 == cpu1) {
//				return 0;
//			}
			
//			float result = ((float)(cpu2-cpu1) / ((cpu2 + idle2) - (cpu1 + idle1)));
			Log.d("CPU", "time perio: " + (System.currentTimeMillis() - start_time));
			double result = ((cpu2)/(((System.currentTimeMillis() - start_time)*996.14/1000)));
			Log.d("CPU", "result: " + result);
			
			return (float) result;
		} catch (IOException ex) {
			ex.printStackTrace();
		}
		
		return -1;
	}

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
				if (firstPlay_time == 0)
					firstPlay_time = System.currentTimeMillis();
				
				if ( BitmapFilledCount[0] >= 0 )
				{
					UIupdater.obtainMessage(ThreadSignal.UPDATE_BUFFER_0).sendToTarget();
				}
				else if ( BitmapFilledCount[0] == -1 ) 
				{
					int W = getWidth();
					int H = getHeight();
					// Log.d("Test H&W",String.valueOf(W)+" "+String.valueOf(H));
					AllocBitmap(W, H);
					SVC_Handler.obtainMessage(ThreadSignal.WRITE_BUFFER_0).sendToTarget();
				} 
				else if (BitmapFilledCount[0] == -2) {
					
				}
//				Log.d("DisplayTesting", "BitmapFilledCount[0] = " + BitmapFilledCount[0]);
				break;
				
			case ThreadSignal.WRITE_BUFFER_1:
				BitmapFilledCount[1] = decodeSVC(updateBitmap[1], bitmapElementNumber);
				if (firstPlay_time == 0) 
					firstPlay_time = System.currentTimeMillis();
				
				if (BitmapFilledCount[1] >= 0) 
				{
					UIupdater.obtainMessage(ThreadSignal.UPDATE_BUFFER_1).sendToTarget();
				}
				else if ( BitmapFilledCount[1] == -1 ) 
				{
					int W = getWidth();
					int H = getHeight();
					// Log.d("Test H&W",String.valueOf(W)+" "+String.valueOf(H));
					AllocBitmap(W, H);
					SVC_Handler.obtainMessage( ThreadSignal.WRITE_BUFFER_1 ).sendToTarget();
				}
				else if (BitmapFilledCount[1] == -2) {
					
				}
//				Log.d("DisplayTesting", "BitmapFilledCount[1] = " + BitmapFilledCount[1]);
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
	public static final boolean FULL_SCREEN = false;
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
	} // End Init

	public void Update(int buffer_index) {
		current_buffer_index = buffer_index;
		int W = this.getWidth();
		int H = this.getHeight();
		
		for (int i = 0; i < BitmapFilledCount[current_buffer_index]; ++i) {
			canvas = viewHolder.lockCanvas();
			
			// clean background
			canvas.drawColor(Color.BLACK);
			
			if (FULL_SCREEN)
			{	// set as a full-screen image, it's will be slower
				Bitmap resizeBmp = Bitmap.createScaledBitmap(updateBitmap[current_buffer_index][i], W, H, false);  
				canvas.drawBitmap(resizeBmp, 0, 0, null);
			}
			else
			{
				canvas.drawBitmap(updateBitmap[current_buffer_index][i], 0, 0, null);
				viewHolder.unlockCanvasAndPost(canvas);
			}
			
			
			/*--- set how long play each frame in GOP ---*/
//			try {
//				Thread.sleep(33);	// default 33
//			} catch (InterruptedException e) {
//				e.printStackTrace(); 
//			}
		} // End for loop
	} // End Update
}
/* End ShowView */
