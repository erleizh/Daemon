package com.erlei.keepalive;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void Daemon(View view) throws IOException {
        String[] files = new String[3];
        for (int i = 0; i < 3; i++) {
            File file = new File(getExternalFilesDir("Program"), "lock" + i);
            if (file.exists()) {
                files[i] = file.getAbsolutePath();
            } else {
                if (file.createNewFile()) {
                    files[i] = file.getAbsolutePath();
                }
            }
        }
        new NativeDaemon().doDaemon(files, "com.erlei.keepalive");
    }

    public void copy(View view) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                install(MainActivity.this,"daemon" ,"daemon","arm64-v8a/daemon");
            }
        }).start();
    }

    private boolean install(Context context, String destDirName, String assetsDirName, String filename) {
        SystemClock.sleep(3000);
        File file = new File(getDir(destDirName, MODE_PRIVATE), filename);
        if (file.exists()) {
            Log.e("Daemon", "文件已存在 :" + file.getAbsolutePath());
            return true;
        }
        try {
            copyAssets(context, (TextUtils.isEmpty(assetsDirName) ? "" : (assetsDirName + File.separator)) + filename, file, "700");
            Log.e("KeepAlive", "拷贝成功 :" + file.getAbsolutePath());
            return true;
        } catch (Exception e) {
            Log.e("KeepAlive", "拷贝失败 :" + file.getAbsolutePath());
            return false;
        }
    }

    private void copyAssets(Context context, String assetsFilename, File file, String mode) throws IOException, InterruptedException {
        AssetManager manager = context.getAssets();
        final InputStream is = manager.open(assetsFilename);
        copyFile(file, is, mode);
    }

    private void copyFile(File file, InputStream is, String mode) throws IOException, InterruptedException {
        if (!file.getParentFile().exists()) {
            file.getParentFile().mkdirs();
        }
        final String abspath = file.getAbsolutePath();
        final FileOutputStream out = new FileOutputStream(file);
        byte buf[] = new byte[1024];
        int len;
        while ((len = is.read(buf)) > 0) {
            out.write(buf, 0, len);
        }
        out.close();
        is.close();
        Runtime.getRuntime().exec("chmod " + mode + " " + abspath).waitFor();
    }


}
