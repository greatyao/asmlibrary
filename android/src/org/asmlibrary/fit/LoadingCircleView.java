package org.asmlibrary.fit;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.View;

/**
 * 鍦嗗舰鍔犺浇杩涘害鏉�
 * 
 * @author way
 * 
 */
public class LoadingCircleView extends View {

	private final Paint paint;
	private final Context context;
	private Resources res;
	private int max = 100;
	private int progress = 0;
	private int ringWidth;
	// 鍦嗙幆鐨勯鑹�
	private int ringColor;
	// 杩涘害鏉￠鑹�
	private int progressColor;
	// 瀛椾綋棰滆壊
	private int textColor;
	// 瀛楃殑澶у皬
	private int textSize;

	private String textProgress;

	public LoadingCircleView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		this.paint = new Paint();
		this.res = context.getResources();
		this.paint.setAntiAlias(true); // 娑堥櫎閿娇
		this.ringWidth = dip2px(context, 10); // 璁剧疆鍦嗙幆瀹藉害
		this.ringColor = Color.BLACK;// 榛戣壊杩涘害鏉¤儗鏅�
		this.progressColor = Color.WHITE;// 鐧借壊杩涘害鏉�
		this.textColor = Color.BLACK;// 榛戣壊鏁板瓧鏄剧ず杩涘害;
		this.textSize = 15;// 榛樿瀛椾綋澶у皬
	}

	public LoadingCircleView(Context context, AttributeSet attrs) {
		this(context, attrs, 0);
	}

	public LoadingCircleView(Context context) {
		this(context, null);
	}

	/**
	 * 璁剧疆杩涘害鏉℃渶澶у�
	 * 
	 * @param max
	 */
	public synchronized void setMax(int max) {
		if (max < 0)
			max = 0;
		if (progress > max)
			progress = max;
		this.max = max;
	}

	/**
	 * 鑾峰彇杩涘害鏉℃渶澶у�
	 * 
	 * @return
	 */
	public synchronized int getMax() {
		return max;
	}

	/**
	 * 璁剧疆鍔犺浇杩涘害锛屽彇鍊艰寖鍥村湪0~涔嬮棿
	 * 
	 * @param progress
	 */
	public synchronized void setProgress(int progress) {
		if (progress >= 0 && progress <= max) {
			this.progress = progress;
			invalidate();
		}
	}

	/**
	 * 鑾峰彇褰撳墠杩涘害鍊�
	 * 
	 * @return
	 */
	public synchronized int getProgress() {
		return progress;
	}

	/**
	 * 璁剧疆鍦嗙幆鑳屾櫙鑹�
	 * 
	 * @param ringColor
	 */
	public void setRingColor(int ringColor) {
		this.ringColor = res.getColor(ringColor);
	}

	/**
	 * 璁剧疆杩涘害鏉￠鑹�
	 * 
	 * @param progressColor
	 */
	public void setProgressColor(int progressColor) {
		this.progressColor = res.getColor(progressColor);
	}

	/**
	 * 璁剧疆瀛椾綋棰滆壊
	 * 
	 * @param textColor
	 */
	public void setTextColor(int textColor) {
		this.textColor = res.getColor(textColor);
	}

	/**
	 * 璁剧疆瀛椾綋澶у皬
	 * 
	 * @param textSize
	 */
	public void setTextSize(int textSize) {
		this.textSize = textSize;
	}

	/**
	 * 璁剧疆鍦嗙幆鍗婂緞
	 * 
	 * @param ringWidth
	 */
	public void setRingWidthDip(int ringWidth) {
		this.ringWidth = dip2px(context, ringWidth);
	}

	/**
	 * 閫氳繃涓嶆柇鐢诲姬鐨勬柟寮忔洿鏂扮晫闈紝瀹炵幇杩涘害澧炲姞
	 */
	@Override
	protected void onDraw(Canvas canvas) {
		int center = getWidth() / 2;
		int radios = center - ringWidth / 2;

		// 缁樺埗鍦嗙幆
		this.paint.setStyle(Paint.Style.STROKE); // 缁樺埗绌哄績鍦�
		this.paint.setColor(ringColor);
		this.paint.setStrokeWidth(ringWidth);
		canvas.drawCircle(center, center, radios, this.paint);
		RectF oval = new RectF(center - radios, center - radios, center
				+ radios, center + radios);
		this.paint.setColor(progressColor);
		canvas.drawArc(oval, 90, 360 * progress / max, false, paint);
		this.paint.setStyle(Paint.Style.FILL);
		this.paint.setColor(textColor);
		this.paint.setStrokeWidth(0);
		this.paint.setTextSize(textSize);
		this.paint.setTypeface(Typeface.DEFAULT_BOLD);
		//textProgress = (int) (1000 * (progress / (10.0 * max))) + "%";
		//float textWidth = paint.measureText(textProgress);
		//canvas.drawText(textProgress, center - textWidth / 2, center + textSize
		//		/ 2, paint);

		super.onDraw(canvas);
	}

	/**
	 * 鏍规嵁鎵嬫満鐨勫垎杈ㄧ巼浠�dp 鐨勫崟浣�杞垚涓�px(鍍忕礌)
	 */
	public static int dip2px(Context context, float dpValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dpValue * scale + 0.5f);
	}

}
