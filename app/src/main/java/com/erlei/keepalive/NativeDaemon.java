package com.erlei.keepalive;

/**
 * Created by lll on 2018/3/6.
 * C 进程守护
 */
public class NativeDaemon {

    static{
        try {
            System.loadLibrary("native_daemon");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public native void doDaemon(String [] args,String serviceName);

}
