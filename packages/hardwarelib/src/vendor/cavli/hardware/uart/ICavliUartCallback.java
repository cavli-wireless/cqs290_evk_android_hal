package vendor.cavli.hardware.uart;

import android.os.RemoteException;
import java.util.ArrayList;

public interface ICavliUartCallback {
    void onDataReceived(ArrayList<Byte> arrayList) throws RemoteException;
}