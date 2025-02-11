<?php ?>

<div class="page-wrapper">

  <div class="nav-mobile">
    <div class="branding">
      <div class="logo"><a href="/"><img src="/sites/all/themes/project/images/logo/logo_t.png" alt="KCCC GROUP" /></a></div>
      <div class="brand">
        <div class="name nowrap"><a href="/">KCCC GROUP</a></div>
        <div class="site-name"><a href="/"><?php print $site_name; ?></a></div>
      </div>
    </div>
    <div class="menu-mobile-wr">
      <?php if ($primary_nav): print $primary_nav; endif; ?>
      <?php if ($secondary_nav): print $secondary_nav; endif; ?>
    </div>
  </div>

  <div class="<?php print $classes; ?>">
    <?php if ($is_header_on): ?>
    <header class="page-header">
      <div class="header-wr">
        <div class="container">
          <div class="row middle-xs full-height no-wrap">
            <div class="col col-1 full-height col-no-gutter">
              <div class="branding">
                <div class="brand">
                  <div class="name"><a href="/">KCCC GROUP</a></div>
                  <div class="site-name"><a href="/"><?php print $site_name; ?></a></div>
                </div>
                <div class="logo"><a href="/"><img src="/sites/all/themes/project/images/logo/logo_t.png" alt="KCCC GROUP" /></a></div>
              </div>
            </div>
            <div class="col col-2 full-height col-no-gutter">
              <div class="menu-bkg">
                <div class="nav-mobile-label hide-lg"><div class="label"><div class="icon"></div></div></div>
                <div class="hide-xs show-lg">
                  <div class="menu-wr">
                    <?php if ($primary_nav): print $primary_nav; endif; ?>
                    <?php if ($secondary_nav): print $secondary_nav; endif; ?>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

    </header>
    <?php endif; ?>

    <div class="page-content">
      <div class="container">

        <?php if ($page['highlighted'] || $is_banner_on): ?>
        <div class="page-highlighted">

          <?php if ($page['highlighted']): ?>
            <?php print render($page['highlighted']); ?>
          <?php endif; ?>

          <?php if ($is_banner_on): ?>
          <div class="page-banner">
            <div class="screen-width">
              <div class="image">
                <picture>
                  <?php if (!empty($banner_mobile_url)): ?><source srcset="<?php print $banner_mobile_url; ?>" media="(max-width: <?php print $banner_break; ?>px)"><?php endif; ?>
                  <img src="<?php print $banner_url; ?>" alt="<?php print $banner_title ?? t('Banner'); ?>">
                </picture>
              </div>
              <div class="container full-height">
                <div class="banner-title-wrapper">
                  <?php if (!empty($banner_title_prefix)): ?><div class="banner-prefix"><?php print $banner_title_prefix; ?></div><?php endif; ?>
                  <?php if (!empty($banner_title)): ?><div class="banner-title"><?php print $banner_title; ?></div><?php endif; ?>
                  <?php if (!empty($banner_title_suffix)): ?><div class="banner-suffix"><?php print $banner_title_suffix; ?></div><?php endif; ?>
                </div>
              </div>
            </div>
          </div>

          <?php endif; ?>
        </div>
        <?php else: ?>
        <div class="page-margin"></div>
        <?php endif; ?>

        <?php print $breadcrumb; ?>

        <div class="page-main">
          <?php if (isset($tabs)): ?><?php print render($tabs); ?><?php endif; ?>
          <?php print $messages; ?>
          <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>

          <?php if ($is_title_on && $title): ?>
            <div class="page-title">
              <?php print render($title_prefix); ?>
              <?php if ($title): ?><h1 class="title" id="page-title"><?php print $title; ?></h1><?php endif; ?>
              <?php print render($title_suffix); ?>
            </div>
          <?php endif; ?>


          <?php print render($page['content']); ?>
        </div>

        <?php if (!empty($page['downlighted'])): ?>
          <div class="page-downlighted">
            <?php print render($page['downlighted']); ?>
          </div>
        <?php endif; ?>

      </div>
    </div>

    <div class="page-footer">
      <div class="container">
        <div class="row middle-xs">
          <div class="col-xs-4">
            <div class="branding"><div class="logo"><a href="/"><img class="logo" src="/sites/all/themes/project/images/logo/logo_t.png" alt="KCCC GROUP" /></a></div></div>
          </div>
          <div class="col-xs-8">
            <div class="rights">© <?php print date('Y', time()); ?> KCCC GROUP</div>
          </div>
        </div>
      </div>
    </div>

  </div>

</div>

