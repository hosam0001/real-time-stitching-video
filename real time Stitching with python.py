import cv2
import numpy as np
import time

class MareArtsStitcher:
    def __init__(self):
        self.m_ImageWarppingWay = "plane"  # or "cylindrical"
        self.m_FeatureFindAlgorithm = "akaze"

    def check_SNcertification(self, email, sn):
        # Implement your SN certification logic here
        return True  # Placeholder

    def calibrateFrame(self, images):
        # Implement your calibration logic here
        return True  # Placeholder

    def stitcher(self, images):
        # Implement your stitching logic here
        # Return a dummy stitched image for testing
        return True, np.zeros((480, 640, 3), dtype=np.uint8)

def initialize_camera(index, width=640, height=480):
    """Initialize a camera with the given index."""
    cap = cv2.VideoCapture(index, cv2.CAP_DSHOW)  # Use CAP_DSHOW for Windows
    if not cap.isOpened():
        print(f"Error: Camera index {index} is not available.")
        return None

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    print(f"Camera {index} initialized with resolution {width}x{height}.")
    return cap

def main():
    # Initialize cameras
    cap1 = initialize_camera(0)
    cap2 = initialize_camera(1)

    if not cap1 or not cap2:
        print("Error: Unable to initialize one or both cameras.")
        return

    # Create OpenCV windows
    cv2.namedWindow("cam1", cv2.WINDOW_NORMAL)
    cv2.namedWindow("cam2", cv2.WINDOW_NORMAL)
    cv2.namedWindow("result", cv2.WINDOW_AUTOSIZE)

    stitcher = MareArtsStitcher()
    MYSN = "your_sn_number"

    # Check SN certification
    if not stitcher.check_SNcertification("my@email.com", MYSN):
        print("SN certification failed.")
        return

    mode = 0
    while True:
        ret1, cameraFrame1 = cap1.read()
        ret2, cameraFrame2 = cap2.read()

        if not ret1 or not ret2:
            print("Error: Failed to grab frames.")
            break

        if mode == 0:
            cv2.imshow("cam1", cameraFrame1)
            cv2.imshow("cam2", cameraFrame2)

        if mode == 1:
            print("Panorama parameter processing...")
            vImg_color = [cameraFrame1, cameraFrame2]
            start_time = time.time()

            if not stitcher.calibrateFrame(vImg_color):
                print("Stitching failed during calibration.")
            else:
                mode = 2

            elapsed_time = time.time() - start_time
            print(f"Processing time = {elapsed_time:.2f} seconds")

        if mode == 2:
            vImg_color = [cameraFrame1, cameraFrame2]
            start_time = time.time()

            success, result = stitcher.stitcher(vImg_color)
            if success:
                cv2.imshow("result", result)

            elapsed_time = time.time() - start_time
            print(f"Processing time = {elapsed_time:.2f} seconds")

        # Keypress event handling
        key = cv2.waitKey(10)
        if key == ord('p'):  # Start processing
            mode = 1
        elif key == ord('r'):  # Reset to camera mode
            mode = 0
        elif key == ord('q'):  # Quit application
            break

    # Release resources
    cap1.release()
    cap2.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
