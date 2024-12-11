<?php
// Set cookie
if (!isset($_COOKIE['test_cookie'])) {
    setcookie("test_cookie", "cookie_value", time() + (86400 * 30), "/"); // 86400 = 1 day
    echo "Cookie has been set.<br>";
} else {
    echo "Cookie Value: " . $_COOKIE['test_cookie'] . "<br>";
}

// Handle Basic Authentication
if (!isset($_SERVER['PHP_AUTH_USER'])) {
    header('WWW-Authenticate: Basic realm="My Protected Area"');
    header('HTTP/1.0 401 Unauthorized');
    echo 'Authentication required.';
    exit;
} else {
    echo "Welcome, " . htmlspecialchars($_SERVER['PHP_AUTH_USER']) . "!<br>";
}

// Simulate Chunked Transfer Encoding
echo "Chunked Transfer-Encoding Test:<br>";
for ($i = 1; $i <= 5; $i++) {
    echo str_repeat("Chunk $i<br>", 1);
    ob_flush();
    flush();
    sleep(1); // Delay to simulate streaming
}
?>

