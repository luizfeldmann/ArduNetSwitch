# Finding OpenSSH app
find_program(OPENSSL openssl
  "$ENV{ProgramFiles}/Git/mingw64/bin")

IF (OPENSSL-NOTFOUND)
  message(FATAL_ERROR "Cannot find OpenSSL needed to generate RSA keys")
ENDIF()

# GENERATION OF PRIVATE KEY
set(PRIVATE_KEY "${CMAKE_CURRENT_LIST_DIR}/private")
set(PRIVATE_PEM "${PRIVATE_KEY}.pem")
set(PRIVATE_DER "${PRIVATE_KEY}.der")

# Generate private PEM
add_custom_command(
  OUTPUT ${PRIVATE_PEM}
  COMMAND "${OPENSSL}" genrsa -out "${PRIVATE_PEM}" 1024
  VERBATIM
)

# Generate private DER
add_custom_command(
  OUTPUT ${PRIVATE_DER}
  DEPENDS ${PRIVATE_PEM}
  COMMAND "${OPENSSL}" rsa -in "${PRIVATE_PEM}" -out "${PRIVATE_DER}" -outform der
  VERBATIM
)

# GENERATION OF PUBLIC KEY
set(PUBLIC_KEY "${CMAKE_CURRENT_LIST_DIR}/public")
set(PUBLIC_PEM "${PUBLIC_KEY}.pem")
set(PUBLIC_DER "${PUBLIC_KEY}.der")

# Generate the public PEM
add_custom_command(
  OUTPUT ${PUBLIC_PEM}
  DEPENDS ${PRIVATE_PEM}
  COMMAND "${OPENSSL}" rsa -in "${PRIVATE_PEM}" -pubout -out "${PUBLIC_PEM}"
  VERBATIM
)

# Generate the public DER
add_custom_command(
  OUTPUT ${PUBLIC_DER}
  DEPENDS ${PUBLIC_PEM}
  COMMAND "${OPENSSL}" rsa -pubin -in "${PUBLIC_PEM}" -out "${PUBLIC_DER}" -outform der
  VERBATIM
)

# Custom target for key generation
add_custom_target(GenerateKeys SOURCES "${PUBLIC_DER}" "${PRIVATE_DER}")