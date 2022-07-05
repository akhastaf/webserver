<?php

if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    // save $_FILES['avatar'] to a inside a folder

   
    $_COOKIE['name'] = $_POST['name'];
    $_COOKIE['email'] = $_POST['email'];
    setcookie('name', $_POST['name'], time() + 3600 * 24 * 7);
    setcookie('email', $_POST['email'], time() + 3600 * 24 * 7);

    } else if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['logout'])) {
    unset($_COOKIE['name']);
    unset($_COOKIE['email']);
    setcookie('name', '', time() - 3600);
    setcookie('email', '', time() - 3600);
}

?>
<!DOCTYPE html>
<div>
    <?php if (isset($_COOKIE['name']) && isset($_COOKIE['email'])): ?>
        <p>
            <center><h1>Hello, <?= $_COOKIE['name'] ?>!</h1></center><hr>
            <center><?php echo $_COOKIE['name']; ?></center>
            <center><?php echo $_COOKIE['email']; ?></center><br/><br/>
            <center><a href="?logout">Logout</a></center>
        </p>
    <?php else: ?>
        <form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" enctype="multipart/form-data">
            <input type="text" name="name" placeholder="Name" />
            <input type="text" name="email" placeholder="Email" />
            <!-- <input type="file" id="avatar" name="avatar" accept="image/png, image/jpeg" /> -->
            <input type="submit" value="Submit" />
        </form>
    <?php endif; ?>
</div>