//Madison Sawvel

import java.util.ArrayList;
import java.util.Random;

class ThreadIt extends Thread {
  static long totalThrown = 0;
  static long totalHits = 0;
  static int threadCount = 0;

  public void run() {
    computeIt();
  }

  public void computeIt() {
    Random rand = new Random();
    long hit = 0;
    long t = 0;
    long dart = 1000000;
    while (t < (dart / threadCount)) {
      float x = rand.nextFloat();
      float y = rand.nextFloat();
      double distance = Math.sqrt(Math.pow((x - .5), 2) + (Math.pow((y - .5), 2)));
      if (distance < .5) {
        hit++;
      }
      t++;
    }
    totalHits = hit;
    totalThrown = t;
  }

  public static void createIt(int times) {
    threadCount = times;
    long startTime = 0, endTime = 0, elapsedTime = 0;
    ArrayList<ThreadIt> threadHold = new ArrayList<ThreadIt>();
    startTime = System.currentTimeMillis();
    for (int create = 0; create < threadCount; create++) {
      ThreadIt thread = new ThreadIt();
      thread.start();
      threadHold.add(thread);
    }
    for (int j = 0; j < threadCount; j++) {
      try {
        threadHold.get(j).join();
      } catch (InterruptedException ie) {
        System.out.println("Thread " + j + " interrupted");
      }
    }
    endTime = System.currentTimeMillis();
    elapsedTime = endTime - startTime;
    System.out.println("Time elapsed to run " + threadCount + " thread(s) in milliseconds: " + elapsedTime);
    System.out.println("Pi = " + (((double) totalHits / totalThrown) * 4));  

  }
}

public class ComputePI {
  public static void main(String args[]) {
    int[] amount = { 1, 2, 4, 8, 16 };
    for (int index = 0; index < amount.length; index++) {
      ThreadIt.createIt(amount[index]);
    }
  }
}
