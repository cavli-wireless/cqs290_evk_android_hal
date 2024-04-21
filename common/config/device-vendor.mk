PRODUCT_PACKAGES += vendor.cavli.hardware.spi@1.0 \
					vendor.cavli.hardware.spi@1.0-service \


# Add cavli manifest
# DEVICE_MANIFEST_FILE += vendor/cavli/device/manifest.xml
DEVICE_MATRIX_FILE += vendor/cavli/device/compatibility_matrix.xml

# Add cavli seplicy
BOARD_SEPOLICY_DIRS += vendor/cavli/sepolicy
