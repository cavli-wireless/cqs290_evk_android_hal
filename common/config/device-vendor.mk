PRODUCT_PACKAGES += vendor.cavli.automotive.vehicle@1.0-service \
					vehicel_client_test \
					cavli-car-service-lib

# Add cavli manifest
# DEVICE_MANIFEST_FILE += vendor/cavli/device/manifest.xml
DEVICE_MATRIX_FILE += vendor/cavli/device/compatibility_matrix.xml

BOARD_SEPOLICY_DIRS += vendor/cavli/sepolicy
