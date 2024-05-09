PRODUCT_PACKAGES += vendor.cavli.hardware.spi@1.0 \
                    vendor.cavli.hardware.spi@1.0-service \
                    spi_client_test \
                    spidev_test \
                    cavli-spi-service-lib \
                    cavlidemo

PRODUCT_PACKAGES += vendor.cavli.hardware.uart@1.0 \
                    vendor.cavli.hardware.uart@1.0-service \
                    uart_client_test

# Add cavli manifest
DEVICE_MANIFEST_FILE += vendor/common/device/manifest.xml

# Set value for DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE didn't work. I work arround by eit
# vendor/qcom/opensource/core-utils/vendor_framework_compatibility_matrix.xml. I should find other
# better way
# DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += vendor/common/device/framework_manifest.xml

# Add cavli seplicy
BOARD_SEPOLICY_DIRS += vendor/common/sepolicy
