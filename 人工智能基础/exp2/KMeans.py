import numpy as np
import matplotlib.pyplot as plt
import cv2


def read_image(filepath="./data/ustc-cow.png"):
    img = cv2.imread(filepath)  # Replace with the actual path to your image
    # Convert the image from BGR to RGB
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img


class KMeans:
    def __init__(self, k=4, max_iter=10):
        self.k = k
        self.max_iter = max_iter

    # Randomly initialize the centers
    def initialize_centers(self, points):
        """
        points: (n_samples, n_dims,)
        """
        n, d = points.shape

        centers = np.zeros((self.k, d))
        for k in range(self.k):
            # use more random points to initialize centers, make kmeans more stable
            random_index = np.random.choice(n, size=10, replace=False)
            centers[k] = points[random_index].mean(axis=0)

        return centers

    # Assign each point to the closest center
    def assign_points(self, centers, points):
        """
        centers: (n_clusters, n_dims,)
        points: (n_samples, n_dims,)
        return labels: (n_samples, )
        """
        n_samples, n_dims = points.shape
        labels = np.zeros(n_samples)
        # TODO: Compute the distance between each point and each center
        # and Assign each point to the closest center
        for i in range(n_samples):
            distances = np.linalg.norm(points[i] - centers, axis=1)
            labels[i] = np.argmin(distances)

        return labels

    # Update the centers based on the new assignment of points
    def update_centers(self, centers, points):
        """
        centers: (n_clusters, n_dims,)
        labels: (n_samples, )
        points: (n_samples, n_dims,)
        return centers: (n_clusters, n_dims,)
        """
        # TODO: Update the centers based on the new assignment of points
        clusters = self.assign_points(centers, points)
        new_centers = np.zeros_like(centers)
        for k in range(self.k):
            new_centers[k] = points[clusters == k].mean(axis=0)

        return new_centers

    # k-means clustering
    def fit(self, points):
        """
        points: (n_samples, n_dims,)
        return centers: (n_clusters, n_dims,)
        """
        # TODO: Implement k-means clustering
        centers = self.initialize_centers(points)
        for _ in range(self.max_iter):
            new_centers = self.update_centers(centers, points)
            if np.allclose(new_centers, centers):
                break
            centers = new_centers
        return centers

    def compress(self, img):
        """
        img: (width, height, 3)
        return compressed img: (width, height, 3)
        """
        # flatten the image pixels
        points = img.reshape((-1, img.shape[-1]))
        # TODO: fit the points and
        # Replace each pixel value with its nearby center value
        centers = self.fit(points)
        labels = self.assign_points(centers, points)
        for i in range(len(points)):
            points[i] = centers[int(labels[i])]
        return points.reshape(img.shape)


if __name__ == "__main__":
    img = read_image(filepath="../data/ustc-cow.png")
    kmeans = KMeans(k=32, max_iter=10000)
    compressed_img = kmeans.compress(img).round().astype(np.uint8)

    plt.figure(figsize=(10, 10))
    plt.imshow(compressed_img)
    plt.title("Compressed Image")
    plt.axis("off")
    plt.savefig("./compressed_image.png")
