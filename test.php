<?php
// Validate Basic Authentication
$valid_username = "admin";
$valid_password = "password";

if (!isset($_SERVER['PHP_AUTH_USER'])) {
    header('WWW-Authenticate: Basic realm="My Protected Area"');
    header('HTTP/1.0 401 Unauthorized');
    echo 'Authentication required.';
    exit;
} elseif ($_SERVER['PHP_AUTH_USER'] !== $valid_username || $_SERVER['PHP_AUTH_PW'] !== $valid_password) {
    header('HTTP/1.0 403 Forbidden');
    echo 'Invalid credentials.';
    exit;
} else {
    echo "Welcome, " . htmlspecialchars($_SERVER['PHP_AUTH_USER']) . "!<br>";
}

// Set cookie if not already set
if (!isset($_COOKIE['test_cookie'])) {
    $cookie_value = htmlspecialchars($_COOKIE['test_cookie'] ?? 'default_cookie');
    setcookie("test_cookie", $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
    echo "Cookie has been set to: $cookie_value<br>";
} else {
    echo "Existing Cookie Value: " . htmlspecialchars($_COOKIE['test_cookie']) . "<br>";
}

// Simulate Chunked Transfer Encoding
echo "Chunked Transfer-Encoding Test:<br>";
for ($i = 1; $i <= 5; $i++) {
    echo "Chunk $i<br>";
    ob_flush();
    flush();
    sleep(1); // Delay to simulate streaming
}
?>

